TARGET = all

all:
	- cd cpu && $(MAKE) $(TARGET)
	- cd memory && $(MAKE) $(TARGET)
	- cd scheduler && $(MAKE) $(TARGET)
	- cd swap && $(MAKE) $(TARGET)
	- echo 'maked'

libs:
	- cd includes && $(MAKE) $(TARGET)

debug: TARGET = debug
debug: all

clean:
	- echo 'TODO'