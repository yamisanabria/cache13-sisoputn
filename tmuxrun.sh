#! /bin/bash
Modulos[0]='swap'
Modulos[1]='memory'
Modulos[2]='scheduler'
Modulos[3]='cpu'


tmux new-window "./cache13modulerun.sh ${Modulos[0]}"
unset hosts[0];
sleep 0
for i in "${Modulos[@]}"; do
  echo "./cache13modulerun.sh $i"
  sleep 0
  tmux split-window -h  "./cache13modulerun.sh $i"
  tmux select-layout tiled > /dev/null
done
tmux select-pane -t 2
#tmux set-window-option synchronize-panes on > /dev/null
