El makefile de test no me funca, pero esto si;

$> make all
$> gcc -L"../src/build" -L"../cspec/release" -o "build/commons-unit-test" ../build/filereader.o build/test_main.o -lcommons -lcspecs
$> ./build/commons-unit-test