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
db = pymysql.connect(host = '172.31.5.78',user = 'root',password='12345',db='testdb',charset='utf8')    #데이터베이스 연결 설정
cur = db.cursor()                                                                                       #데이터베이스 커서 설정
HOST = '172.31.3.23'                                                                                    #IP번호
PORT = 8900                                                                                             #포트번호

i = 0                                                                                                   #각종 변수 초기화 및 선언
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
    client_socket.connect((HOST, PORT))                                                                 #소켓연결시작
    rev_date = client_socket.recv(1024)                                                                 #서버에서 보낸값 받기
    unpack_data = struct.unpack('H',  rev_date)                                                         #서버에서 보낸 바이트값을 H(정수)형식 으로 변경
    if unpack_data[0] == 1:                                                                             #정수형식으로 변경된 값이 1일때 실행시켜줄 조건문
        socket_lable1.config(background='lime green')                                                   #소켓이 활성화되었다는 것을 보여주는 GUI 변경
        phtoo2.config(file="start.png")                                                                 #버튼을 활성화 시켜준다는것을 보여주는 GUI 변경
        phtoo3.config(file="stop.png")
        socket_textbox.delete(0,'end')                                                                  #소켓이 활성화되었다는 것을 보여주는 텍스트 변경
        socket_textbox.insert(0,'on')
    refreshToken()                                                                                      #토큰값 리프레쉬
def moohan():
    try:
        global x                                                                                        #글로벌 변수 선언
        global i
        global j
        global k
        global date
        global plus_time
        zero_timer = 0                                                                                  #변수 초기화
        msg = 'a'
        client_socket.sendall(msg.encode())                                                             #클라이언트에서 소켓으로 데이터를 보내주는 구문
        while True:
            today_data = datetime.now()                                                                 #현재 날짜를 받아오는 코드
            data = client_socket.recv(1024)                                                             #서버에서 값을 받아오는 코드
            date = struct.unpack('HHHHH',  data)                                                       
            sql = "INSERT INTO testtable VALUES('"+str(date[0])+"','"+str(date[1])+"','"+str(date[2])+"','"\
                +str(date[3])+"','"+str(today_data.strftime('%y-%m-%d %H:%M:%S'))+"')"                  #데이터베이스의 쿼리문을 사용하기 위해 쿼리문을 미리 변수에 저장
            cur.execute(sql)                                                                            #데이터 베이스의 쿼리문을 호출하는 코드
            db.commit()                                                                                 #데이터 베이스에 쿼리문을 적용하는 코드
            sql = "SELECT * FROM testtable"                                                             #쿼리문 재설정
            cur.execute(sql)
            row= cur.fetchall()                                                                         #데이터 베이스에 저장되어있는 모든데이터 가져오기
            db_data.insert(i,row[-1])                                                                   #데이터 베이스에 저장 되는 값 중 제일 마지막 값 가져와서 저장
            sql = "SELECT time FROM testtable"                                                          #쿼리문 재설정
            cur.execute(sql)
            row1= cur.fetchall()                                                                        
            db_data2.insert(i,row1[-1]) 
            start_time = db_data2[0].__getitem__(0)                                                     #튜플,리스트 형태로 저장되어있는 요소 값 추출
            current_time = db_data2[i].__getitem__(0)
            difference_time = db_data2[i-1].__getitem__(0)
            start_time_textbox.delete(0,'end')                                                          #시간 표현 GUI 텍스트 변경
            start_time_textbox.insert(0,start_time)
            current_time_textbox.delete(0,'end')
            current_time_textbox.insert(0,current_time)
            out = "-: "                                                                                 #저장된 요소값에서 제거힐 요소 선정
            start_time = ''.join( x for x in start_time if x not in out)                                #요소값에서 제거할요소 제거후 다시 저장
            start_time = int((start_time[10:]))
            current_time = ''.join( x for x in current_time if x not in out)
            current_time = int((current_time[10:]))
            difference_time = ''.join( x for x in difference_time if x not in out)
            difference_time = int((difference_time[10:]))
            plus_time.insert(-1,current_time - difference_time)                                         #실시간 시간 시간 변동을 구현하기 위한 코드
            if plus_time[i] != plus_time[i-1]:
                zero_timer = zero_timer + 1
            timer = [int(zero_timer / 36000),int((zero_timer/ 3600)% 10),int((zero_timer/ 600)% 6),int((zero_timer/60) % 10),int((zero_timer % 60)/10),int(zero_timer %10)] # 실시간 구동 시간 구현
            timer = ("{0}{1}:{2}{3}:{4}{5}".format(timer[0],timer[1],timer[2],timer[3],timer[4],timer[5]))
            timer_textbox.delete(0,'end')
            timer_textbox.insert(0,timer)
            if k > 0:                                                                                   #그래프를 그릴 X,Y값 범위지정을 위한 조건문
                del x_location[0]
                del y_location[0]
                del x_speed[0]
                del y_speed[0]
                del time_data[0]
                del error_data[0]
            x_location.insert(i,date[0])                                                                #1번그래프 1번 Y값
            y_location.insert(i,date[1])                                                                #1번그래프 2번 Y값
            x_speed.insert(i,date[2])                                                                   #2번그래프 1번 Y값
            y_speed.insert(i,date[3])                                                                   #2번그래프 2번 Y값
            error_data.insert(i,date[4])                                                                #에러 코드값
            time_data.insert(i,today_data.strftime("%M:%S"))                                            #1,2번 X값
        
            ax = fig.add_subplot(1,1,1)                                                                 #그래프 표현을 해주는 코드
            bx = figg.add_subplot(1,1,1)                                                                
            ax.plot(time_data[:i] ,x_location[:i],'r',label='X_location')                               #선을 표현해주는 코드(X값,Y값,색상,텍스트)표현
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
            db_date = ("| X_location: %s Y_location: %s X_speed: %s Y_speed: %s Time: %s |" % db_data[i])               #데이터베이스에 저장된값 불러오기 후 GUI텍스트박스에 표현
            db_message_box.insert(0,db_date)
            ax.set_title('Motor Location')                                                                              #1번 그래프 타이틀 표현
            ax.set_ylabel('Y: Motor Location')                                                                          #1번 그래프 Y값 이름 표현
            ax.set_xlabel('X: time {0} (Minute),(Second)'.format(today_data.strftime('%y-%m-%d')))                      #1번 그래프 X값 이름 표현
            bx.set_title('Motor Speed')                                                                                 #2번 그래프 타이틀 표현
            bx.set_ylabel('Y: Motor Speed')                                                                             #2번 그래프 Y값 이름 표현
            bx.set_xlabel('X: time {0} (Minute),(Second)'.format(today_data.strftime('%y-%m-%d')))                      #2번 그래프 X값 이름 표현
            ax.legend(loc='upper left',fontsize=8)                                                                      #그래프속 선 색상 상자 표현
            bx.legend(loc='upper left',fontsize=8)
            canvas.draw()                                                                                               #그래프 구현
            canvas.flush_events()
            canvass.draw()
            canvass.flush_events()
            fig.clear()                                                                                                 #그래프 업데이트를 위한 그래프 삭제
            figg.clear()
            if date[4] == 999:                                                                                          #에러발생 조건문
                socket_lable1.config(background='red')
                socket_textbox.delete(0,'end')
                socket_textbox.insert(0,'off')
                msgbox.showerror('Error','에러발생!')
                kakao_text()
                naver_email()
                client_socket.close()
            if j == 10:                                                                                                 #X,Y배열값 조절을 위한 조건문
                j = 0
                k = k + 1
            if x == False:
                break
            i=i+1
            j=j+1
            client_socket.sendall(data)                                                                                 #서버에서 데이터가 정확하게 전달되었는지 확인 하기위해 서버에서 받은값 다시 전송하는 코드
            time.sleep(0.1)
    except OSError:                                                                                                     #소켓이 연결되지않았을때 예외처리
        msgbox.showerror('Connect Error','연결 에러 발생!')

def stop():                                                                                                             #정지 버튼을 누르면 발동되는 커멘드 구현
    global x
    try:
        socket_lable1.config(background='red')
        phtoo2.config(file="disable_start.png")
        phtoo3.config(file="disable_stop.png")
        socket_textbox.delete(0,'end')
        socket_textbox.insert(0,'off')
        x = False
        db.close()                                                                                                      #데이터베이스 종료
        client_socket.close()                                                                                           #소켓종료
    except pymysql.err.Error:
        pass
def exit():                                                                                                             #종료버튼 구현
    root.destroy()                                                                                                      #GUI창 종료 코드

def tokens():                                                                                                           #KAKAO메세지를위한 토큰 발급 리프레쉬가 되지않았을때 토큰값을 발급해야함
    url = "https://kauth.kakao.com/oauth/token"

    data = {
        "grant_type" : "authorization_code",
        "client_id" : "82f6d607662b80e45a70472dfc754ba4",
        "redirect_uri" : "https://localhost.com",
        "code"         : "_7m2OuFmu_AF_7Eg9rpkJ49H55TjfJRsc660wMT-ncNG2d-Yu8pa_LYFU45tvKfsltRDhQo9dGkAAAF6g3aXpw"
    }
    response = requests.post(url, data=data)

    tokens = response.json()

    with open("kakao_token.json", "w") as fp:
        json.dump(tokens, fp)

def refreshToken():                                                                                                     #토큰값 리프레쉬
    with open("kakao_token.json","r") as fp:
        token = json.load(fp)
    REST_API_KEY = "82f6d607662b80e45a70472dfc754ba4"
    url = "https://kauth.kakao.com/oauth/token"

    data = {
        "grant_type": "refresh_token",                                                                                  # 얘는 단순 String임. "refresh_token"    
        "client_id":f"{REST_API_KEY}",
        "refresh_token": token['refresh_token']                                                                         # 여기가 위에서 얻은 refresh_token 값
    }    
 
    resp = requests.post(url , data=data)
    token['access_token'] = resp.json()['access_token']
    with open("kakao_token.json", "w") as fp:
        json.dump(token, fp)
    return(token)

def kakao_text():                                                                                                       #KAKAO메세지를 보내는 코드
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

                                                                                                             
    text = f"""\                                                                    
    Error code : {list_time[0]} error code'{date[4]}' 에러가 발생했습니다. 기동을 중지합니다.
    """
    template = {
    "object_type": "text",
    "text": text,
    "link": {
        "web_url": weather_url,
        "mobile_web_url": weather_url
    }                                                                                                                   # "button_title": "날씨 상세보기"
    }                                                                                                                   # 날씨 정보 만들기

                                                                                                                  
    payload = {
        "template_object" : json.dumps(template)
    }                                                                                                                   # JSON 형식 -> 문자열 변환 

                                                                                                                 
    res = requests.post(kakaotalk_template_url, data=payload, headers=kheaders)                                         # 카카오톡 보내기

    if res.json().get('result_code') == 0:
        kakao_textbox.delete(0,'end')
        kakao_textbox.insert(0,'send')
        message_box.insert(0,'kakao_message :'+text)
    else:
        print('메시지를 성공적으로 보내지 못했습니다. 오류메시지 : ' + str(res.json()))

def naver_email():                                                                                                      # NAVER메세지를 보내는 코드
    today_time = datetime.today()
    list_time = []
    list_time.insert(0,today_time.strftime('%y-%m-%d %H:%M:%S'))     
    def send_email(smtp_info, msg):
        with smtplib.SMTP(smtp_info["smtp_server"], smtp_info["smtp_port"]) as server:                                  # TLS 보안 연결
            server.starttls() 
                                                                                                                  
            server.login(smtp_info["smtp_user_id"], smtp_info["smtp_user_pw"])                                          # 로그인

                                                                            
            response = server.sendmail(msg['from'], msg['to'], msg.as_string())                                         # 로그인 된 서버에 이메일 전송, 메시지를 보낼때는 .as_string() 메소드를 사용해서 문자열로 바꿔줍니다.
            
            if not response:
                naver_textbox.delete(0,'end')
                naver_textbox.insert(0,'send')
                message_box.insert(1,'naver_message :'+content)
            else:
                print(response)

    from email.mime.text import MIMEText

    smtp_info = dict({"smtp_server" : "smtp.naver.com",                                                                 # SMTP 서버 주소
                    "smtp_user_id" : "keeess9999@naver.com", 
                    "smtp_user_pw" : "6954hkhkhk", 
                    "smtp_port" : 587})                                                                                 # SMTP 서버 포트
    title = "에러 메일입니다."                                                                                           # 메일 내용 작성
    content = f"""\
    Error code : {list_time[0]} error code'{date[4]}' 에러가 발생했습니다. 기동을 중지합니다.
    """
    sender = "keeess9999@naver.com"
    receiver = "keeess9999@naver.com"                              
    msg = MIMEText(_text = content, _charset = "utf-8")                                                                 # 메일 객체 생성 : 메시지 내용에는 한글이 들어가기 때문에 한글을 지원하는 문자 체계인 UTF-8을 명시해줍니다.
                                                                                                                    
    msg['Subject'] = title                                                                                              # 메일 제목
    msg['From'] = sender                                                                                                # 송신자
    msg['To'] = receiver                                                                                                # 수신자

    send_email(smtp_info, msg )

root = Tk()                                                                                                             #GUI화면 구현 코드
root.title("2축 모터 PC PLC 통신 프로그램")         
root.geometry("1050x1050+100+0")                                                                                        #GUI창 크기 와 시작할때 켜지는 좌표값
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)                                                       #소켓변수 선언
label_1 = Label(root,text="2축 모터 PC PLC 통신",font=("Mordern",15))                                                    #GUI화면 구현 코드들
label_1.place(x=280)
phtoo = PhotoImage(file="connect.png")
button1= Button(root,image=phtoo,command=click,borderwidth=0).place(x=800,y=310) 
phtoo2 = PhotoImage(file="disable_start.png")
button2= Button(root,image=phtoo2,command=moohan,borderwidth=0).place(x=800,y=410) 
phtoo3 = PhotoImage(file="disable_stop.png")
button3= Button(root,image=phtoo3,command=stop,borderwidth=0).place(x=800,y=510) 
phtoo4 = PhotoImage(file="exit.png")
button4= Button(root,image=phtoo4,command=exit,borderwidth=0).place(x=800,y=610) 
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
start_time_label = Label(root,text="시작시간",font=('맑은 고딕',12))
start_time_label.place(x=780,y=770)
start_time_textbox = Entry(root,font=('맑은 고딕',12))
start_time_textbox.place(x=780,y=800,width=250,height=25)
current_time_label = Label(root,text="현재시간",font=('맑은 고딕',12))
current_time_label.place(x=780,y=850)
current_time_textbox = Entry(root,font=('맑은 고딕',12))
current_time_textbox.place(x=780,y=880,width=250,height=25)
timer_label = Label(root,text="가동시간",font=('맑은 고딕',12))
timer_label.place(x=780,y=930)
timer_textbox = Entry(root,font=('맑은 고딕',12))
timer_textbox.place(x=780,y=960,width=250,height=25)
fig = plt.figure(figsize=(7,4))                                                                                     #그래프 그릴 창 생성
figg = plt.figure(figsize=(7,4))
canvas = FigureCanvasTkAgg(fig, master=root)                                                                        #그래프를 그리기 위한 캔버스 선언
canvass = FigureCanvasTkAgg(figg, master=root)
canvas.get_tk_widget().place(x=50,y=30)                                                                             #그래프 나타내줄 캔버스 위치
canvass.get_tk_widget().place(x=50,y=440)
root.mainloop()                                                                                                     
#GUI종료
