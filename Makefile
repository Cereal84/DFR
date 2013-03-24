all: 
	@echo
	@echo Compilazione parte DFR
	@echo
	cd ./src_DFR; make 
	@echo
	@echo Compilazione parte Client 
	@echo
	cd ./src_CL; make 
	@echo
	@echo Compilazione parte File Server
	@echo
	cd ./src_FS; make 
	@echo

dfr:
	@echo
	@echo Compilazione parte DFR 
	@echo
	cd ./src_DFR; make 
	@echo

cl:
	@echo
	@echo Compilazione parte Client 
	@echo
	cd ./src_CL; make 
	@echo

fs:
	@echo
	@echo Compilazione parte File Server 
	@echo
	cd ./src_FS; make 
	@echo 

clean:
	cd ./src_common; make clean
	cd ./src_CL; make clean
	cd ./src_FS; make clean
	cd ./src_DFR; make clean
