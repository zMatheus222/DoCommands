#!/bin/bash

case "$1" in
    start)
        sudo systemctl start docommands
        ;;
    stop)
        sudo systemctl stop docommands
        ;;
    restart)
        sudo systemctl restart docommands
        ;;
    status)
        sudo systemctl status docommands
        ;;
    *)
        echo "Uso: $0 {start|stop|restart|status}"
        exit 1
        ;;
esac

exit 0
