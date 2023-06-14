ps -elf | grep agentd | awk '{print $4}'  | while read line; do kill $line; done
ps -elf | grep runtime_locking_shardd | awk '{print $4}'  | while read line; do kill $line; done
ps -elf | grep ticket_machined | awk '{print $4}'  | while read line; do kill $line; done