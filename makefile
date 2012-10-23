
all: microcreator/microcreator microlaunch/microlaunch microdetect/microdetect

microcreator/microcreator:
	cd microcreator; make 


microlaunch/microlaunch:
	cd microlaunch; make 

microdetect/microdetect:
	cd microdetect; make

clean:
	cd microlaunch; make clean; cd ../microdetect; make clean; cd ../microcreator; make clean
