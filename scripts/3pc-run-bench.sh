#!/bin/bash
IP="localhost"
PORT="8080"
LOGLEVEL="WARN"
TRANSFER="erc20"
ACCOUNTS="8192"


function print_help() {
    echo "Usage: 3pc-run-bench.sh [OPTIONS]"
    echo ""
    echo "OPTIONS:"
    echo "  --ip           The IP address to use. Default is localhost."
    echo "  --port         The port number to use. Default is 8888."
    echo "  --loglevel     The log level to use. Default is WARN."
    echo "  --transfer     The Transfer Type.(transfer, erc20)"
    echo "  -h, --help     Show this help message and exit."
    echo ""
}

for arg in "$@"; do
    if [[ "$arg" == "-h" || "$arg" == "--help" ]]; then
        print_help
        exit 0
    elif [[ "$arg" == "--ip"* ]]; then 
        IP="${arg#--ip=}"
    elif [[ "$arg" == "--port"* ]]; then
        PORT="${arg#--port=}"
    elif [[ "$arg" == "--loglevel"* ]]; then
        LOGLEVEL="${arg#--loglevel=}"
    elif [[ "$arg" == "--transfer"* ]]; then
        TRANSFER="${arg#--transfer=}"
    elif [[ "$arg" == "--account"* ]]; then
        ACCOUNTS="${arg#--account=}"
    fi
done

mkdir -p logs
echo targeting agent on $IP:$PORT
echo Log level = $LOGLEVEL
echo Transfer type = $TRANSFER
echo Gen Address Count = $ACCOUNTS


./scripts/wait-for-it.sh -s $IP:$PORT -t 60 -- ./build/tools/bench/3pc/evm/evm_bench --shard_count=1 --shard0_count=1 --shard00_endpoint=$IP:5556 --node_id=0 --component_id=0 --agent_count=1 --agent0_endpoint=$IP:$PORT --ticket_machine_count=1 --ticket_machine0_endpoint=$IP:7777 --loadgen_accounts=$ACCOUNTS --loadgen_txtype=$TRANSFER --telemetry=1

