# weather-mr

A stream-based MapReduce example for processing WBAN weather information
The data files for testing can be downloaded from https://www.dropbox.com/sh/78jminnuolneb19/AAAvcrSIfCx16YwiM5ftxVfGa?dl=0

- **awk.sh** - Sample script to test how long it would take to process the whole set of data using one single computer with awk. 
- **mapper.sh** - Uses awk to parse the WBAN raw data (csv)
 
	- Input: Raw WBAN hourly temperature data (https://www.dropbox.com/sh/78jminnuolneb19/AAAvcrSIfCx16YwiM5ftxVfGa?dl=0) 
	- Output: Date;WBAN;DryBulbCelsius;Time

 	- More Info:
		- All the fields in the file header
		- WBAN - http://www2.epa.gov/exposure-assessment-models/weather-bureau-army-navy-wban-station-locations-and-30-year-normals
		- DryBulbCelsius - https://en.wikipedia.org/wiki/Dry-bulb_temperature

- **reducer.pl**: Takes data with format Date;WBAN;DryBulbCelsius;Time ordered by DATE;WBAN and returns the day, the WBAN, the greater temperature and what time that measure was sampled

	- Input (ordered!) DATE;WBAN;TEMP;TIME
	- Output: DATE;WBAN;MAX_DAILY_TEMP_IN_WBAN;TIME_OF_MEASUREMENT

## Example

### awk.sh
```
	time zcat *hourly.txt.gz | ./awk.sh
```

### Map/Reduce scripts

This is a way to test the scripts. Of course ***the best performance would be obtained by executing them THROUGH a MapReduce job over Hadoop***

```
	time zcat *hourly.txt.gz | ./mapper.sh | sort | ./reduce.pl
```

