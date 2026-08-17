Install MYSQL

mysql
CREATE DATABASE succubus;
quit;
mysql -u root -pYOURPASSWORD succubus < database.sql

Change sql user and pass in config/succubus.json

ulimit -n100000
screen ./main