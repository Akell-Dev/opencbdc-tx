#!/bin/bash

LOG_FILE_PATH="logs/"
CONF_FILE_PATH="./test.cfg"
IP="localhost"

function print_help() {
    echo "Usage: 3pc-run-local-with-config-file.sh [OPTIONS]"
    echo ""
    echo "OPTIONS:"
    echo "  --log      Set Log file path"
    echo "  --file     Set configure file PATH"
    echo "  -h, --help     Show this help message and exit."
    echo ""
}

for arg in "$@"; do
    if [[ "$arg" == "-h" || "$arg" == "--help" ]]; then
        print_help
        exit 0
    elif [[ "$arg" == "--log"* ]]; then
        LOG_FILE_PATH="${arg#--log=}"
    elif [[ "$arg" == "--file"* ]]; then 
        CONF_FILE_PATH="${arg#--file=}"
    fi
done


echo Running $CONF_FILE_PATH 
echo Logging $LOG_FILE_PATH

mkdir -p $LOG_FILE_PATH
./build/src/3pc/runtime_locking_shard/runtime_locking_shardd 0 0 $CONF_FILE_PATH $LOG_FILE_PATH/shardd.log &
sleep 1
./scripts/wait-for-it.sh -s $IP:5556 -t 60 -- ./build/src/3pc/ticket_machine/ticket_machined 0 0 $CONF_FILE_PATH $LOG_FILE_PATH/ticket_machined.log &
sleep 1
./scripts/wait-for-it.sh -s $IP:7777 -t 60 -- ./scripts/wait-for-it.sh -s $IP:5556 -t 60 -- ./build/src/3pc/agent/agentd 0 0 $CONF_FILE_PATH $LOG_FILE_PATH/agentd0.log &
sleep 1
./scripts/wait-for-it.sh -s $IP:7777 -t 60 -- ./scripts/wait-for-it.sh -s $IP:5556 -t 60 -- ./build/src/3pc/agent/agentd 1 0 $CONF_FILE_PATH $LOG_FILE_PATH/agentd1.log &

