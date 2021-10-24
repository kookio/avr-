from datetime import datetime
from email import message
from email.mime import text
import json
from logging import Handler
import time
import tkinter
from tkinter import font
from types import TracebackType
from pymysql.cursors import Cursor
import requests
from email.mime.text import MIMEText
import tkinter.messagebox as msgbox
import socket
import struct
import pymysql
import smtplib
from tkinter import *
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
db = None
cur = None
db = pymysql.connect(host = '172.31.5.78',user = 'root',password='12345',db='testdb',charset='utf8')
cur = db.cursor()
HOST = '172.31.3.23'
PORT = 8900
i = 0
j = 0
k = 0
x = True
date = []
x_location = []
y_location = []
x_speed = []
y_speed = []
time_data = []
error_data = []
db_data = []
db_data2 = []
plus_time = []


def click():
    client_socket.connect((HOST, PORT))
    rev_date = client_socket.recv(1024)
    unpack_data = struct.unpack('H',  rev_date)
    if unpack_data[0] == 1: 
        socket_lable1.config(background='lime green')
        phtoo2.config(file="start.png")
        phtoo3.config(file="stop.png")
        socket_textbox.delete(0,'end')
        socket_textbox.insert(0,'on')
    tokens()
def moohan():
    try:
        global x
        global i
        global j
        global k
        global date
        global plus_time
        zero_timer = 0
        msg = 'a'
        client_socket.sendall(msg.encode())
        while True:
            today_data = datetime.now()
            data = client_socket.recv(1024)
            date = struct.unpack('HHHHH',  data) 
            # print('Received', data.decode()) 
            # print('Received', int.from_bytes(data,byteorder='big',lengt=32,offset=4))
            sql = "INSERT INTO testtable VALUES('"+str(date[0])+"','"+str(date[1])+"','"+str(date[2])+"','"+str(date[3])+"','"+str(today_data.strftime('%y-%m-%d %H:%M:%S'))+"')"
            cur.execute(sql)
            db.commit()
            sql = "SELECT * FROM testtable"
            cur.execute(sql)
            row= cur.fetchall()
            db_data.insert(i,row[-1])
            sql = "SELECT time FROM testtable"
            cur.execute(sql)
            row1= cur.fetchall()
            db_data2.insert(i,row1[-1]) 
            start_time = db_data2[0].__getitem__(0)
            current_time = db_data2[i].__getitem__(0)
            difference_time = db_data2[i-1].__getitem__(0)
            # timer1= i / 36000
            # timer2 = (i/ 3600)% 10
            # timer3 = i/ 600
            # timer4 = (i/60) % 10
            # timer5 = (i % 60)/10
            # timer6 = i %10
            # timer = ("{0}{1}:{2}{3}:{4}{5}".format(int(timer1),int(timer2),int(timer3),int(timer4),int(timer5),int(timer6)))
            start_time_textbox.delete(0,'end')
            start_time_textbox.insert(0,start_time)
            current_time_textbox.delete(0,'end')
            current_time_textbox.insert(0,current_time)
            out = "-: "
            start_time = ''.join( x for x in start_time if x not in out)
            start_time = int((start_time[10:]))
            current_time = ''.join( x for x in current_time if x not in out)
            current_time = int((current_time[10:]))
            difference_time = ''.join( x for x in difference_time if x not in out)
            difference_time = int((difference_time[10:]))
            plus_time.insert(-1,current_time - difference_time)
            if plus_time[i] != plus_time[i-1]:
                zero_timer = zero_timer + 1
            timer = [int(zero_timer / 36000),int((zero_timer/ 3600)% 10),int((zero_timer/ 600)% 6),int((zero_timer/60) % 10),int((zero_timer % 60)/10),int(zero_timer %10)]
            timer = ("{0}{1}:{2}{3}:{4}{5}".format(timer[0],timer[1],timer[2],timer[3],timer[4],timer[5]))
            timer_textbox.delete(0,'end')
            timer_textbox.insert(0,timer)
            if k > 0:
                del x_location[0]
                del y_location[0]
                del x_speed[0]
                del y_speed[0]
                del time_data[0]
                del error_data[0]
            x_location.insert(i,date[0])
            y_location.insert(i,date[1])
            x_speed.insert(i,date[2])
            y_speed.insert(i,date[3])
            error_data.insert(i,date[4])
            time_data.insert(i,today_data.strftime("%M:%S"))
        
            ax = fig.add_subplot(1,1,1)
            bx = figg.add_subplot(1,1,1)
            ax.plot(time_data[:i] ,x_location[:i],'r',label='X_location')
            ax.plot(time_data[:i] ,y_location[:i],'b',label='Y_location')
            bx.plot(time_data[:i] ,x_speed[:i],'r',label='X_speed')
            bx.plot(time_data[:i] ,y_speed[:i],'b',label='Y_speed')
            textbox.delete(0,"end")
            textbox.insert(0,date[0])
            textbox.update()
            textbox_1.delete(0,"end")
            textbox_1.insert(0,date[1])
            textbox_1.update()
            textbox_2.delete(0,"end")
            textbox_2.insert(0,date[4])
            textbox_2.update()
            db_date = ("| X_location: %s Y_location: %s X_speed: %s Y_speed: %s Time: %s |" % db_data[i])
            db_message_box.insert(0,db_date)
            ax.set_title('Motor Location')
            ax.set_ylabel('Y: Motor Location')
            ax.set_xlabel('X: time {0} (Minute),(Second)'.format(today_data.strftime('%y-%m-%d')))
            bx.set_title('Motor Speed')
            bx.set_ylabel('Y: Motor Speed')
            bx.set_xlabel('X: time {0} (Minute),(Second)'.format(today_data.strftime('%y-%m-%d')))
            ax.legend(loc='upper left',fontsize=8)
            bx.legend(loc='upper left',fontsize=8)
            canvas.draw()
            canvas.flush_events()
            canvass.draw()
            canvass.flush_events()
            fig.clear()
            figg.clear()
            if date[4] == 999:
                socket_lable1.config(background='red')
                phtoo2.config(file="disable_start.png")
                phtoo3.config(file="disable_stop.png")
                socket_textbox.delete(0,'end')
                socket_textbox.insert(0,'off')
                msgbox.showerror('Error','에러발생!')
                kakao_text()
                naver_email()
                client_socket.close()
            if j == 10:
                j = 0
                k = k + 1
            if x == False:
                break
            i=i+1
            j=j+1
            client_socket.sendall(data)
            time.sleep(0.1)
    except OSError:
        msgbox.showerror('Connect Error','통신 연결을 확인해주세요.')

def stop():
    global x
    try:
        socket_lable1.config(background='red')
        phtoo2.config(file="disable_start.png")
        phtoo3.config(file="disable_stop.png")
        socket_textbox.delete(0,'end')
        socket_textbox.insert(0,'off')
        x = False
        db.close()
        client_socket.close()
    except pymysql.err.Error:
        pass
def exit():
    root.destroy()

def tokens():
    url = "https://kauth.kakao.com/oauth/token"

    data = {
        "grant_type" : "authorization_code",
        "client_id" : "82f6d607662b80e45a70472dfc754ba4",
        "redirect_uri" : "https://localhost.com",
        "code"         : "a0MERmrcudqygFwPYIEuk78lKk8kGuH_6Lsrf5C9icHKFimnZfCFkoYZ-9RdkStZPYc4_Qopb9QAAAF6iQMYOQ"
    }
    response = requests.post(url, data=data)
    
    tokens = response.json()

    with open("kakao_token.json", "w") as fp:
        json.dump(tokens, fp)

def refreshToken():
    with open("kakao_token.json","r") as fp:
        token = json.load(fp)
    REST_API_KEY = "82f6d607662b80e45a70472dfc754ba4"
    url = "https://kauth.kakao.com/oauth/token"

    data = {
        "grant_type": "refresh_token", # 얘는 단순 String임. "refresh_token"    
        "client_id":f"{REST_API_KEY}",
        "refresh_token": token['refresh_token'] # 여기가 위에서 얻은 refresh_token 값
    }    
 
    resp = requests.post(url , data=data)
    token['access_token'] = resp.json()['access_token']
    with open("kakao_token.json", "w") as fp:
        json.dump(token, fp)
    return(token)

def kakao_text():
    today_time = datetime.today()
    list_time = []
    list_time.insert(0,today_time.strftime('%y-%m-%d %H:%M:%S')) 
    
    with open("kakao_token.json","r") as fp:    
        tokens = json.load(fp)
    kcreds = {
        "access_token" : tokens.get('access_token')
    }
    kheaders = {
        "Authorization": "Bearer " + kcreds.get('access_token')
    }

    kakaotalk_template_url = "https://kapi.kakao.com/v2/api/talk/memo/default/send"

    # 날씨 상세 정보 URL
    weather_url = "https://search.naver.com/search.naver?sm=top_hty&fbm=0&ie=utf8&query=%EB%82%A0%EC%94%A8"

    # 날씨 정보 만들기 
    text = f"""\
    Error code : {list_time[0]} error code'{date[4]}' 에러가 발생했습니다. 기동을 중지합니다.
    """
    template = {
    "object_type": "text",
    "text": text,
    "link": {
        "web_url": weather_url,
        "mobile_web_url": weather_url
    }
    # "button_title": "날씨 상세보기"
    }

    # JSON 형식 -> 문자열 변환
    payload = {
        "template_object" : json.dumps(template)
    }

    # 카카오톡 보내기
    res = requests.post(kakaotalk_template_url, data=payload, headers=kheaders)

    if res.json().get('result_code') == 0:
        kakao_textbox.delete(0,'end')
        kakao_textbox.insert(0,'send')
        message_box.insert(0,'kakao_message :'+text)
    else:
        print('메시지를 성공적으로 보내지 못했습니다. 오류메시지 : ' + str(res.json()))

def naver_email():
    today_time = datetime.today()
    list_time = []    
    list_time.insert(0,today_time.strftime('%y-%m-%d %H:%M:%S')) 
    def send_email(smtp_info, msg):
        with smtplib.SMTP(smtp_info["smtp_server"], smtp_info["smtp_port"]) as server:
            # TLS 보안 연결
            server.starttls() 
            # 로그인
            server.login(smtp_info["smtp_user_id"], smtp_info["smtp_user_pw"])

            # 로그인 된 서버에 이메일 전송
            response = server.sendmail(msg['from'], msg['to'], msg.as_string()) # 메시지를 보낼때는 .as_string() 메소드를 사용해서 문자열로 바꿔줍니다.
            
            if not response:
                naver_textbox.delete(0,'end')
                naver_textbox.insert(0,'send')
                message_box.insert(1,'naver_message :'+content)
            else:
                print(response)

    from email.mime.text import MIMEText

    smtp_info = dict({"smtp_server" : "smtp.naver.com", # SMTP 서버 주소
                    "smtp_user_id" : "keeess9999@naver.com", 
                    "smtp_user_pw" : "6954hkhkhk", 
                    "smtp_port" : 587}) # SMTP 서버 포트

    # 메일 내용 작성
    title = "에러 메일입니다."
    content = f"""\
    Error code : {list_time[0]} error code'{date[4]}' 에러가 발생했습니다. 기동을 중지합니다.
    """
    sender = "keeess9999@naver.com"
    receiver = "keeess9999@naver.com" 

    # 메일 객체 생성 : 메시지 내용에는 한글이 들어가기 때문에 한글을 지원하는 문자 체계인 UTF-8을 명시해줍니다.
    msg = MIMEText(_text = content, _charset = "utf-8") # 메일 내용

    msg['Subject'] = title     # 메일 제목
    msg['From'] = sender       # 송신자
    msg['To'] = receiver       # 수신자

    send_email(smtp_info, msg )

root = Tk()
root.title("Motor Socket Program")
root.geometry("1050x1050+100+0")
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
label_1 = Label(root,text="Motor Location,Speed Graph",font=("함초롱",20  ))
label_1.place(x=220)


phtoo = PhotoImage(file="connect.png")
button1= Button(root,image=phtoo,command=click,borderwidth=0).place(x=800,y=360) 
phtoo2 = PhotoImage(file="disable_start.png")
button2= Button(root,image=phtoo2,command=moohan,borderwidth=0).place(x=800,y=460) 
phtoo3 = PhotoImage(file="disable_stop.png")
button3= Button(root,image=phtoo3,command=stop,borderwidth=0).place(x=800,y=560) 
phtoo4 = PhotoImage(file="exit.png")
button4= Button(root,image=phtoo4,command=exit,borderwidth=0).place(x=800,y=660) 
frame3 = LabelFrame(root,relief="solid",text="Status",bd=1,padx=5,pady=3,font=('System'))
frame3.place(x=795,y=50)
socket_text = Label(frame3,text="Socket Status :",font=('맑은 고딕',12))
socket_text.grid(column=0,row=0)
socket_lable1 = Label(frame3,background='red',relief="flat",bd =1 ,width=10,height=1)
socket_lable1.grid(column=1,row=0)
socket_lable2 = Label(frame3,text='Socket connect:',font=('맑은 고딕',12))
socket_lable2.grid(column=0,row=1)
socket_textbox = Entry(frame3,width=10)
socket_textbox.grid(column=1,row=1)
socket_textbox.insert(0,'off')
label_2 = Label(frame3,text="X location : ",font=('맑은 고딕',12))
label_2.grid(column=0,row=2)
textbox = Entry(frame3,width=10)
textbox.grid(column=1,row=2)
textbox.insert(0,'None')
label_3 = Label(frame3,text="Y location : ",font=('맑은 고딕',12))
label_3.grid(column=0,row=3)
textbox_1 = Entry(frame3,width=10)
textbox_1.grid(column=1,row=3)
textbox_1.insert(0,'None')
label_4 = Label(frame3,text="Error code : ",font=('맑은 고딕',12))
label_4.grid(column=0,row=4)
textbox_2 = Entry(frame3,width=10)
textbox_2.grid(column=1,row=4)
textbox_2.insert(0,'None')
kakao_label = Label(frame3,text="Kakao message : ",font=('맑은 고딕',12))
kakao_label.grid(column=0,row=5)
kakao_textbox = Entry(frame3,width=10)
kakao_textbox.grid(column=1,row=5)
kakao_textbox.insert(0,'Not send')
naver_label = Label(frame3,text="Naver message : ",font=('맑은 고딕',12))
naver_label.grid(column=0,row=6)
naver_textbox = Entry(frame3,width=10)
naver_textbox.grid(column=1,row=6)
naver_textbox.insert(0,'Not send')
frame4 = LabelFrame(root,text="DB_Data",relief="solid",bd=0,padx=5,pady=5)
frame4.place(x=40,y=850)
frame5 = LabelFrame(root,text="Error_Send_Message",relief="solid",bd=0,padx=5,pady=5)
frame5.place(x=40,y=940)
message_box = Listbox(frame5,width=100,height=3)
message_box.grid(column=0,row=0)
db_message_box = Listbox(frame4,width=100,height=4)
db_message_box.grid(column=0,row=0)
db_message_box_scrollbar= Scrollbar(frame4,orient="vertical")
db_message_box_scrollbar.grid(column=1,row=0)
db_message_box_scrollbar.config(command=db_message_box.yview)

start_time_label = Label(root,text="Start time",font=('맑은 고딕',11))
start_time_label.place(x=775,y=840)
start_time_textbox = Entry(root,font=('맑은 고딕',12))
start_time_textbox.place(x=780,y=870,width=250,height=25)
current_time_label = Label(root,text="Current time",font=('맑은 고딕',11))
current_time_label.place(x=775,y=900)
current_time_textbox = Entry(root,font=('맑은 고딕',12))
current_time_textbox.place(x=780,y=930,width=250,height=25)
timer_label = Label(root,text="Operating time",font=('맑은 고딕',11))
timer_label.place(x=775,y=960)
timer_textbox = Entry(root,font=('맑은 고딕',12))
timer_textbox.place(x=780,y=990,width=250,height=25)
fig = plt.figure(figsize=(7,4))  #그리프 그릴 창 생성
figg = plt.figure(figsize=(7,4))
canvas = FigureCanvasTkAgg(fig, master=root)
canvass = FigureCanvasTkAgg(figg, master=root)
canvas.get_tk_widget().place(x=50,y=30)
canvass.get_tk_widget().place(x=50,y=440)
root.mainloop()
 