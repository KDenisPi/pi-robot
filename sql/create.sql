BEGIN TRANSACTION;
CREATE TABLE measurement(
	mdate TEXT PRIMARY KEY NOT NULL,
	hum INTEGER,
	temp INTEGER,
	pressure INTEGER,
	lux INTEGER,
	co2 INTEGER,
	tvoc INTEGER,
	altitude INTEGER);
COMMIT;
.exit


