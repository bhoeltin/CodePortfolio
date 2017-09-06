#!/usr/bin/env python2.6
import string
import sys
import re
#import requests
import os

# the following is the HTML used to create the output page

pageTemplate= '''<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html style="background-color:rgb(0, 0, 84)">
<head>
<meta content="text/html; charset=ISO-8859-1"http-equiv="content-type">
<title> Welcome! </title>
<h1 style="color:white" align="left"> Dining Hall Meal Planner </h1>
</head>
<body style="background-color:rgb(0, 0, 84)">


<h3 style="color:white" align="left"> Meal Item #1 </h3>

<p style="color:white" align="left">  Item: {T1} </p>
<p style="color:white" align="left">  Calories: {A1} </p>
<img src={url1}  align="middle"  width="128" height="128" />

<h3 style="color:white" align="left"> Meal Item #2 </h3>

<p style="color:white" align="left">  Item: {T2} </p>
<p style="color:white" align="left">  Calories: {A2} </p>
<img src={url2} width="128" height="128" align="middle"/>

<h3 style="color:white" align="left"> Meal Item #3 </h3>

<p style="color:white" align="left">  Item: {T3} </p>
<p style="color:white" align="left">  Calories: {A3} </p>
<img src={url3}  width="128" height="128" align="middle"/>

<h3 style="color:white" align="left"> Meal Item #4 </h3>

<p style="color:white" align="left">  Item: {T4} </p>
<p style="color:white" align="left">  Calories: {A4} </p>
<img src={url4} width="128" height="128" align="middle"/>

<h3 style="color:white" align="left"> Meal Item #5 </h3>

<p style="color:white" align="left">  Item: {T5} </p>
<p style="color:white" align="left">  Calories: {A5} </p>
<img src={url5} width="128" height="128" align="middle"/>

<p style="color:white"> Total Calorie Count: {Cals}</p>
<p style="color:white"> {warn} </p>
</body>
</html>
'''


def strToFile(text, filename):
    output = open("Breakfast.html","w")
    output.write(text)
    output.close()

def browseLocal(webpageText, filename='Breakfast.html'):
    import webbrowser, os.path
    strToFile(webpageText, filename)
    webbrowser.open("file:///" + os.path.abspath(filename))

#default URLs for the pictures                                   
url1 = "\"http://trichilofoods.com/site/wp-content/uploads/2015/06/veggies.jpg\""
url2 = "\"http://trichilofoods.com/site/wp-content/uploads/2015/06/veggies.jpg\"" 
url3 = "\"http://trichilofoods.com/site/wp-content/uploads/2015/06/veggies.jpg\""
url4 = "\"http://trichilofoods.com/site/wp-content/uploads/2015/06/veggies.jpg\"" 
url5 = "\"http://trichilofoods.com/site/wp-content/uploads/2015/06/veggies.jpg\""


urlcount = -1
# parsing through the url.txt to get the urls of the pictures
with open('url.txt','r') as f:
    for line in f.readlines():
        line = line.rstrip()
        line = '\"' + line + '\"'
	if urlcount == 0 and "http" in line:
		url1 = line
	if urlcount == 1 and "http" in line:
		url2 = line
	if urlcount == 2 and "http" in line:
		url3 = line
	if urlcount == 3 and "http" in line:
		url4 = line
	if urlcount == 4 and "http" in line:
		url5 = line		
	urlcount = urlcount+1


#VARIABLES TO BE USED IN FUNCTIONS
T1 = "N/A"
A1 = "N/A"
T2 = "N/A"
A2 = "N/A"
T3 = "N/A"
A3 = "N/A"
T4 = "N/A"
A4 = "N/A"
T5 = "N/A"
A5 = "N/A"
warn = ""

# parameters for webpage
strings = []
linecount = 0
for line in sys.stdin:
    if (linecount == 0):
        line = line.split('!')
        Cals = line[0]
        s2 = line[1]
    if ('ov' in s2):
        warn = "Your calorie count is higher than you wanted! Consider making healthier decsisions!"
    elif ('no' in s2):
        warn = "Enjoy your meal!!"
    if (linecount == 1):
        line = line.split('!')
        T1 = line[0]
        A1 = line[1]
        T1 = T1.replace('+',' ')
    if (linecount == 2):
        line = line.split('!')
        T2 = line[0]
        A2 = line[1]
        T2 = T2.replace('+',' ')
    if (linecount == 3):
        line = line.split('!')
        T3 = line[0]
        A3 = line[1]
        tbl = string.maketrans(' ','+')
        T3 = T3.replace('+',' ')
    if (linecount == 4):
        line = line.split('!')
        T4 = line[0]
        A4 = line[1]
        T4 = T4.replace('+',' ')
    if (linecount == 5):
        line = line.split('!')
        T5 = line[0]
        A5 = line[1]
        T5 = T5.replace('+',' ')
    linecount=linecount + 1
contents = pageTemplate.format(**locals())
browseLocal(contents)
