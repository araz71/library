rm -rf bin
mkdir bin
cp extract_number.txt bin/
cd bin
cmake ../
make
./Test
