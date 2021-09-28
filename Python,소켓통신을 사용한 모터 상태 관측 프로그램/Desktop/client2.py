import socket
import struct
import pymysql
import time


db = None
cur = None
db = pymysql.connect(host = '172.31.5.78',user = 'root',password='12345',db='testdb',charset='utf8')
cur = db.cursor()

HOST = '172.31.3.23'
PORT = 8900
#통신할 소켓 오픈 
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#서버 accept()에 연결요청. server와 같은 host, 같은 port 이어야함
client_socket.connect((HOST, PORT))

while True:
    msg = input('msg:')
    client_socket.sendall(msg.encode())	#서버로 msg전송
    data = client_socket.recv(1024)
    date = struct.unpack('HH',  data)
    # print('Received', data.decode())
    print('Received', data)
    # print('Received', int.from_bytes(data,byteorder='big',lengt=32,offset=4))
    print('Received', date)
    dates = list(date)
    sql = "INSERT INTO testtable VALUES('"+str(dates[0])+"','"+str(dates[1])+")"
    cur.execute(sql)
    if msg=='' :
        break
db.commit()
db.close()
client_socket.close()