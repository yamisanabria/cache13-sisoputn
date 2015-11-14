export LD_LIBRARY_PATH=$(pwd)/includes:$LD_LIBRARY_PATH
echo Corriendo $1
cd "$1"
./"$1"
