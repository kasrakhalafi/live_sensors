from flask import Flask, jsonify, render_template, request
from random import *
import threading
import time

app = Flask(__name__, static_url_path='')



@app.route('/js/<path:path>')
def send_js(path):
    return send_from_directory('js', path)



@app.route('/_change')
def add_numbers33():
    a = request.args.get('a', 0, type=int)
    xx=jsonify(result=a)

    print("\nValue is:%s\n"%xx)
    return xx

@app.route('/postjson', methods = ['POST'])
def postJsonHandler():
    global MY_JSON_CONTENTS

    content = request.get_json()
    MY_JSON_CONTENTS=content

    #print (content)
    print("nodeName is :%s"%content['sensorName'])
    print("sensorType is :%s"%content['sensorType'])
    print("Values are :%s"%content['values'])
    print("Times are :%s"%content['timestamps'])
    print("cond are :%s"%content['condition'])

    return 'JSON posted'

@app.route('/_ref')
def refs():
    global MY_JSON_CONTENTS
    print("Refs")
    #return jsonify(0);
    return jsonify(myresult=MY_JSON_CONTENTS)

@app.route('/')
def index():
    return render_template('index.html')

app.run(port=8080,threaded=True,host='0.0.0.0');
