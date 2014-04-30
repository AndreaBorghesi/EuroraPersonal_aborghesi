#!/bin/sh
ssh aborghes@login.plx.cineca.it
screen
ssh -L login1.plx.cineca.it:33022:rvn06:3302 aborghes@rvn06
logout
logout




## LaST creation time --> 2014/04/15 08:00 circa
