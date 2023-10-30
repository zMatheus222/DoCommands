#include "main.h"

using namespace std;

bool CommandScheduler::show_logs = false;

string CommandScheduler::all_commands_string;
set<string> CommandScheduler::unique_commands;

void CommandScheduler::wait_ms(int time) {
	this_thread::sleep_for(chrono::milliseconds(time));
}

void CommandScheduler::log_wait(const string& text){
	cout << "\033[33m" << text << "\033[0m" << endl;
	string var; getline(cin >> ws, var);
}

string CommandScheduler::pick_os(){
	
	string os;
    FILE* stream = popen("cat /etc/os-release", "r");

    if (stream) {
        char buffer[128];
        while (!feof(stream)) {
            if (fgets(buffer, 128, stream) != nullptr) {
                os += buffer;
            }
        }
        pclose(stream);
    } else {
		cerr << "Erro ao tentar definir tipo de sistema." << endl;
    }
	
	bool is_ubuntu = regex_search(os, regex("ubuntu|Ubuntu"));
	bool is_centos = regex_search(os, regex("centos|CentOS"));
	
	string final_so;
	
	if(is_ubuntu){
		final_so = "ubuntu";
	}
	if(is_centos){
		final_so = "centos";
	}
	
	return final_so;
}

void CommandScheduler::limpar_tela(){
	system("clear");
}

string CommandScheduler::gerar_id(){

	const string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	const int idLength = 10;

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> distribution(0, charset.size() - 1);

	string randomID;

	for (int i = 0; i < idLength; ++i) {
		randomID += charset[distribution(gen)];
	}

	return randomID;
}

vector<vector<string>> CommandScheduler::definir_esquema(){

	vector<vector<string>> all_commands(50);

	//ifstream arquivo_yml("commands.yml"); //redhat
	ifstream arquivo_yml(pwd_executavel() + "/commands.yml"); //ubuntu
	
	if (!arquivo_yml.is_open()) {
		cout << "falha ao carregar o arquivo .yml, verifique se o mesmo existe." << endl;
		exit(0);
	}
	else {

		//passando os comandos do arquivo .yml para um vetor
		int count = 0;
		string line;
		while (getline(arquivo_yml, line))
		{
			smatch sma_line;
			if (regex_search(line, sma_line, regex("^(?!#)(.*)\\|(?:(\\d{1,2})\\-(\\d{2})\\-(\\d{4}))\\|(?:(\\d{2})\\:(\\d{2})\\:(\\d{2}))"))) {

				all_commands[count].push_back(gerar_id());
				all_commands[count].push_back(sma_line[1]);
				all_commands[count].push_back(sma_line[2]);
				all_commands[count].push_back(sma_line[3]);
				all_commands[count].push_back(sma_line[4]);
				all_commands[count].push_back(sma_line[5]);
				all_commands[count].push_back(sma_line[6]);
				all_commands[count].push_back(sma_line[7]);

				count++;
			}
		}
		arquivo_yml.close();
	}

	return all_commands;
}

vector<string> CommandScheduler::calc_left_time(string &actual_time, string &command_time) {
	
	struct tm tm1 = {};
	struct tm tm2 = {};
	string return_string;

	// Converter as strings em objetos 'tm'
	strptime(actual_time.c_str(), "%H:%M:%S", &tm1);
	strptime(command_time.c_str(), "%H:%M:%S", &tm2);

	// Converter os objetos 'tm' para segundos desde a meia-noite
	time_t act_time = mktime(&tm1);
	time_t comm_time = mktime(&tm2);

	// Calcular a diferença em segundos
	int diff = abs(static_cast<int>(comm_time - act_time));

	// Converter a diferença em horas, minutos e segundos
	int hours = diff / 3600;
	int minutes = (diff % 3600) / 60;
	int seconds = diff % 60;

	//definir como a parte de hora deve aparecer
	//return_string = to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
	//return return_string;

	vector<string> time_and_condition;
	stringstream ss;

	ss << setfill('0') << setw(2) << hours << ":" << setfill('0') << setw(2) << minutes << ":" << setfill('0') << setw(2) << seconds;

	time_and_condition.push_back(ss.str());

	if (hours < 1) {
		time_and_condition.push_back("near");
	}
	else if ((hours < 1) && (minutes < 10)) {
		time_and_condition.push_back("veryclose");
	}
	else {
		time_and_condition.push_back("taketime");
	}

	return time_and_condition;
}

void CommandScheduler::pos_execucao(int& count_1, vector<vector<string>>& all_commands) {

	system("clear");

	//log_wait("after clear, before ofstream log_file");

	ofstream log_file(pwd_executavel() + "/commands.log", ios::app);
	if (!log_file.is_open()) {
		cout << "Arquivo commands.log não foi encontrado!.";
		exit(0);
	}
	else {

		string log_string = "Command.............: - " + all_commands[count_1][1] + "\n"
							"DCID................: " + all_commands[count_1][0] + "\n" +
			                "Day of execution....: " + all_commands[count_1][2] + "\\" + all_commands[count_1][3] + "\\" + all_commands[count_1][4] + "\n" +
			                "When will executed..: " + all_commands[count_1][5] + ":" + all_commands[count_1][6] + ":" + all_commands[count_1][7] + "\n";

		//log_wait("after string log_string text");

		log_file << log_string;

		//log_wait("after log_file << log_string");

		log_file.close();
	}

	//log_wait("before all_commands.erase(all_commands.begin() + count_1);");

	//remover item apos execução, movendo o iterator para o primeiro item do vetor, depois count_1 é a posiçaõ que queremos remover
	all_commands.erase(all_commands.begin() + count_1);
	//diminuir em 1 count afinal removemos um indice do vetor.
	count_1--;

	//log_wait("after all_commands.erase(all_commands.begin() + count_1);");

}

void CommandScheduler::show_on_screen_comparation(int count_1, vector<string> &time_data, vector<vector<string>> &all_commands) {

	const string COLOR_RESET = "\033[0m";
	const string RED = "\033[31m";
	const string ORANGE = "\033[33m";
	const string YELLOW = "\033[33m";
	const string GREEN = "\033[32m";
	const string GRAY = "\033[90m";
	const string BLUE = "\033[34m";
	const string CYAN = "\033[36m";

	string TIME_LEFT_COLOR;

	//calcular tempo restante para executar o comando.
	string actual_time = time_data[3] + ":" + time_data[4] + ":" + time_data[5];
	string command_time = all_commands[count_1][5] + ":" + all_commands[count_1][6] + ":" + all_commands[count_1][7];
	vector<string> time_left = calc_left_time(actual_time, command_time);

	if (time_left[1] == "near") {
		TIME_LEFT_COLOR = YELLOW;
	}
	else if (time_left[1] == "veryclose") {
		TIME_LEFT_COLOR = ORANGE;
	}
	else {
		TIME_LEFT_COLOR = COLOR_RESET;
	}

	string cout_string
		= "Command.............: - " + CYAN + all_commands[count_1][1] + COLOR_RESET + "\n"
		  "DCID................: " + GRAY + all_commands[count_1][0] + COLOR_RESET + "\n"
		+ "Day of execution....: " + all_commands[count_1][2] + "\\" + all_commands[count_1][3] + "\\" + all_commands[count_1][4] + "\n"
		+ "When will executed..: " + all_commands[count_1][5] + ":" + all_commands[count_1][6] + ":" + all_commands[count_1][7] + "\n"
		+ "Time left to execute: " + TIME_LEFT_COLOR + time_left[0] + COLOR_RESET + "\n\n";

	if (unique_commands.find(all_commands[count_1][0]) == unique_commands.end()) {
		// Comando único, adicione à string e ao conjunto
		all_commands_string += cout_string;
		unique_commands.insert(all_commands[count_1][0]);
	}

}

int CommandScheduler::transform_mes(string mes_curto){
	
	if(regex_search(mes_curto, regex("(Jan|jan)"))){
		return 1;
	}
	if(regex_search(mes_curto, regex("(Feb|feb)"))){
		return 2;
	}
	if(regex_search(mes_curto, regex("(Mar|mar)"))){
		return 3;
	}
	if(regex_search(mes_curto, regex("(Apr|apr|Abr|abr)"))){
		return 4;
	}
	if(regex_search(mes_curto, regex("(May|may|Mai|mai)"))){
		return 5;
	}
	if(regex_search(mes_curto, regex("(Jun|jun)"))){
		return 6;
	}
	if(regex_search(mes_curto, regex("(Jul|jul)"))){
		return 7;
	}
	if(regex_search(mes_curto, regex("(Aug|aug|Ago|ago)"))){
		return 8;
	}
	if(regex_search(mes_curto, regex("(Sep|sep|Set|set)"))){
		return 9;
	}
	if(regex_search(mes_curto, regex("(Oct|oct|Out|out)"))){
		return 10;
	}
	if(regex_search(mes_curto, regex("(Nov|nov)"))){
		return 11;
	}
	if(regex_search(mes_curto, regex("(Dec|dec)"))){
		return 12;
	}

	return 0;
}

vector<string> CommandScheduler::pegar_data_horario(){
	if (show_logs) { cout << "entered on pegar_data_horario()" << endl; }

	//executar comando date, e retornar o resultado dele para variavel "sys_hour"
	FILE* pipe = popen("date", "r");

	char buffer[128];
	string sys_hour = "";
	while (fgets(buffer, sizeof(buffer), pipe) != NULL)
	{
		sys_hour += buffer;
	}

	pclose(pipe);
	//
	
	//detectar qual é o SO, pois o date retorna valores diferentes no ubuntu e centos por exemplo
	
	string os = pick_os();
	regex rgx_date;
	vector<string> time_data(6);
	
	if(os == "centos"){
		rgx_date = "([A-Za-z]+)\\s([A-Za-z]+)\\s(\\d+)\\s(?:(\\d{2}):(\\d{2}):(\\d{2}))\\s([A-Za-z]+)\\s([A-Za-z]+\\s)?(\\d{4})";
		
		smatch sma_h;
		regex_search(sys_hour, sma_h, rgx_date);

		//substituir Oct retornado no comando para 10. assim os valores podem se equivaler na comparação
		string day = sma_h[3];
		string month; if (sma_h[2] == "Oct") { month = "10";}
		string year = sma_h[9];
		string hour = sma_h[4];
		string minute = sma_h[5];
		string second = sma_h[6];

		time_data[0] = day; //dia
		time_data[1] = month; //mes
		time_data[2] = year;
		time_data[3] = hour;
		time_data[4] = minute;
		time_data[5] = second;
	}
	if(os == "ubuntu"){
		
		rgx_date = "([A-Za-z]+)\\s(\\d+)\\s([A-Za-z]+)\\s(\\d{4})\\s(?:(\\d{2}):(\\d{2}):(\\d{2}))\\s\\-\\d{1,3}";

		smatch sma_h;
		regex_search(sys_hour, sma_h, rgx_date);

		//substituir Oct retornado no comando para 10. assim os valores podem se equivaler na comparação
		string day = sma_h[2];
		string month = to_string(transform_mes(sma_h[3]));
		string year = sma_h[4];
		string hour = sma_h[5];
		string minute = sma_h[6];
		string second = sma_h[7];

		time_data[0] = day; //dia
		time_data[1] = month; //mes
		time_data[2] = year;
		time_data[3] = hour;
		time_data[4] = minute;
		time_data[5] = second;
	}
	
	return time_data;
}

void CommandScheduler::executar_comandos(vector<vector<string>>& all_commands){
	while (true){
		//variaveis que cuidam do que é exibido na tela
		all_commands_string = "";
		unique_commands.clear();

		int count_1 = 0;
		for (vector<string> actual_command : all_commands)
		{
			int count_2 = 0;
			for (string actual_command_data : actual_command)
			{
				//pegar o horario toda vez que for verificar
				vector<string> time_data = pegar_data_horario();

				show_on_screen_comparation(count_1, time_data, all_commands);

				//if(time_data[0] == all_commands[count_1][2]){ log_wait("aaaa");}
				//if(time_data[1] == all_commands[count_1][3]){ log_wait("bbbb");}
				//if(time_data[2] == all_commands[count_1][4]){ log_wait("cccc");}
				//if(time_data[3] == all_commands[count_1][5]){ log_wait("dddd");}
				
				//cout << "time_data[1]: " << time_data[1] << "\nall_commands[" << count_1 << "][3]: " << all_commands[count_1][3] << endl;
				

				//se bater ano, mes, dia, hora, minuto, segundo entre o que foi definido no .yml e o horario atual executa o comando.
				if ((time_data[0] == all_commands[count_1][2]) && (time_data[1] == all_commands[count_1][3]) && (time_data[2] == all_commands[count_1][4]) && (time_data[3] == all_commands[count_1][5]) && (time_data[4] == all_commands[count_1][6]) && (time_data[5] == all_commands[count_1][7]))
				{		
					system(all_commands[count_1][1].c_str());
					pos_execucao(count_1, all_commands);
					//log_wait("pre cout <<");
					cout << "Comando dcid: [" + all_commands[count_1][0] + "] executando: " << all_commands[count_1][1] << endl;
					//log_wait("after pos_executio, cout <<");
				}
				count_2++;
			}
			count_1++;

		}
		limpar_tela();
		cout << all_commands_string;
		wait_ms(500);
	}

}

string CommandScheduler::pwd_executavel(){
	
	// Obtém o caminho absoluto do executável
	char result[PATH_MAX];
	
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	char* dir_name;
	if (count != -1) {
		result[count] = '\0';

		// Extrai o diretório pai do caminho absoluto
		dir_name = dirname(result);
		cout << "Pasta raiz do executável: " << dir_name << std::endl;
	}
	
	return dir_name;
}

void CommandScheduler::inicializador() {

	while (true){
		
		limpar_tela();

		cout << "Do Command v0.1\n" << endl;
		cout << "Iniciando..." << endl;
		
		wait_ms(1500);

		vector<vector<string>> all_commands;
		vector<string> time_data;

		all_commands = definir_esquema();
		executar_comandos(all_commands);
	}
	
}

int main() {
	
	CommandScheduler obj_cs;
	obj_cs.inicializador();

	return 0;
}
