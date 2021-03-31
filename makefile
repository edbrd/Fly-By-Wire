-include ./makefile.init
RM := rm -rf
-include ./makefile.defs

# All Target
all: mainProject

# Tool invocations
mainProject:
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	mkdir -p log
	mkdir -p bin
	mkdir -p tmp
	gcc -c -MMD -MF"bin/PFC.d" -MT"bin/PFC.o" -o "bin/PFC.o" "src/PFC.c"
	gcc -c -MMD -MF"bin/getSetLasts.d" -MT"bin/getSetLasts.o" -o "bin/getSetLasts.o" "src/getSetLasts.c"
	gcc -c -MMD -MF"bin/getsG18.d" -MT"bin/getsG18.o" -o "bin/getsG18.o" "src/getsG18.c"
	gcc -c -MMD -MF"bin/mainProject.d" -MT"bin/mainProject.o" -o "bin/mainProject.o" "src/mainProject.c"
	gcc -c -MMD -MF"bin/transducer.d" -MT"bin/transducer.o" -o "bin/transducer.o" "src/transducer.c"
	gcc -c -MMD -MF"bin/disconnectSwitch.d" -MT"bin/disconnectSwitch.o" -o "bin/disconnectSwitch.o" "src/disconnectSwitch.c"
	gcc -c -MMD -MF"bin/wes.d" -MT"bin/wes.o" -o "bin/wes.o" "src/wes.c"
	gcc  -o "mainProject" ./bin/PFC.o ./bin/getSetLasts.o ./bin/getsG18.o ./bin/mainProject.o ./bin/transducer.o ./bin/disconnectSwitch.o ./bin/wes.o -lm
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	rm -rf ./bin/PFC.o ./bin/getSetLasts.o ./bin/getsG18.o ./bin/mainProject.o ./bin/transducer.o ./bin/disconnectSwitch.o ./bin/wes.o ./bin/PFC.d ./bin/getSetLasts.d ./bin/getsG18.d ./bin/mainProject.d ./bin/transducer.d ./bin/disconnectSwitch.d ./bin/wes.d  mainProject
	-rm -rf tmp
	-rm -rf log
	-rm -rf bin
.PHONY: all clean dependents
.SECONDARY:

-include ./makefile.targets
