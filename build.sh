if [ ! -d "./build" ]; 
then
    mkdir "build";
fi

cd "./build";
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_GENERATOR_PLATFORM=x64 ..;

if cmake --build .; then
    echo "Cmake build successful."
else
    echo "Cmake build failed."
    exit 1
fi