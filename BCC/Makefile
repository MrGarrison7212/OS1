SRC = ..\test\*.cpp ..\src\*.cpp 
INCLUDES = ..\test\*.h ..\h\*.h ..\*.h 
LIBS = ..\*.lib 
INCLUDEPATH = C:\bc31\include;..\test;..\h;..\;
LIBSPATH = C:\bc31\lib;..\;
TARGET = ProjekatOS.exe
$(TARGET):	$(SRC) $(INCLUDES) configuration.xmi
	bcc.exe -mh -v -e$(TARGET) -I$(INCLUDEPATH) -L$(LIBSPATH) $(SRC) $(LIBS)
all:	$(TARGET)
clean:
	-del *.obj
	-del $(TARGET)
	-del *.asm
configuration.xmi:	
	configuration.xmi