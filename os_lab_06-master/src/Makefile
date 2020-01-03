all: calculator controller

calculator: calculator.cpp
	g++ calculator.cpp -o calculator -lzmq

controller: controller.cpp ctrl.h
	g++ controller.cpp -o controller -lzmq -lpthread