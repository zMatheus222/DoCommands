#ifndef DOCOMMANDS_HPP
#define DOCOMMANDS_HPP

#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <cstdio>
#include <vector>
#include <ctime>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <set>
#include <random>

//caminho arquivo
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
//caminho arquivo

//para criar um processo (C)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//para criar um processo (C)

class CommandScheduler {

public:

	void inicializador();

private:

	static void limpar_tela();
	static void wait_ms(int time);
	static void log_wait(const std::string& text);
	std::vector<std::vector<std::string>> definir_esquema();
	static std::vector<std::string> calc_left_time(std::string& actual_time, std::string& command_time);
	void show_on_screen_comparation(int count_1, std::vector<std::string>& time_data, std::vector<std::vector<std::string>>& all_commands);
	std::vector<std::string> pegar_data_horario();
	void executar_comandos(std::vector<std::vector<std::string>>& all_commands);
	void pos_execucao(int& count_1, std::vector<std::vector<std::string>>& all_commands);
	std::string gerar_id();
	static std::string pwd_executavel();
	std::string pick_os();
	int transform_mes(std::string mes_curto);

	static std::string all_commands_string;
	static std::set<std::string> unique_commands;
	static bool show_logs;
};

#endif // DOCOMMANDS_HPP
