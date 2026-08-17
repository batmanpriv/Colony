create database `richard`;

use richard;

CREATE TABLE `users` (
    `username` varchar(32) NOT NULL,
    `password` varchar(32) NOT NULL,
    KEY `username` (`username`)
);

INSERT INTO users(username, password) VALUES('admin', 'password69!');
