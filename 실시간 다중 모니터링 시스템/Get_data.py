"""
데이터 수집 ver_1.0.5
----------------------------------------
PLC-PC 통신 연결
FINS Commands로 데이터 송수신
Maria DB를 이용한 데이터베이스 축적
멀티스레드를 이용한 다중접속 구현
서버 접속 시 상태 변수 생성

"""

## import modules
import socket
import binascii
import struct
from bitstring import BitArray
import mariadb
import threading
import time
import sys
from Data import *
import datetime
from UI import Error_Window_PLC_A,Error_Window_PLC_B,TimeOut_Error_Window,ConnectionRefused_Error_Window
from kakao_msg import kakao_send
# data store_global
# PLC-A
check_server_a = ['failed']
datetime_a = [0 for i in range(0, 10)]
Belt_Pos_a = [0 for i in range(0, 10)]
Belt_Vel_a = [0 for i in range(0, 10)]
Circle_Pos_a = [0 for i in range(0, 10)]
Circle_Vel_a = [0 for i in range(0, 10)]
EC_error_code_a= []
MC_error_code_a = []

# PLC-B
check_server_b = ['failed']
datetime_b = [0 for i in range(0, 10)]
Belt_Pos_b = [0 for i in range(0, 10)]
Belt_Vel_b = [0 for i in range(0, 10)]
Circle_Pos_b = [0 for i in range(0, 10)]
Circle_Vel_b = [0 for i in range(0, 10)]
EC_error_code_b = []
MC_error_code_b = []




# PLC-Bool(Run) data
bool_data_a = [0]
bool_data_b = [0]




## sepuence
# Get_data_PLC_A
class PLC_A(threading.Thread):
    def __init__(self):
        super().__init__()
        self.daemon = True
        self.running = True
        self.thread_name = 'Thread_1'
        self.plc_name = 'PLC-A'
        self.format = '(' + self.thread_name + ' ' + self.plc_name + ')'
        self.now = datetime.datetime.now()
        self.time_format = '[' + self.now.strftime('%X') + ']'
        self.running = True

    def run(self):
        # socket 초기값
        HOST_1 = '172.31.7.11'
        PORT_1 = 9600
        client_socket_1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        global check_server_a
        global bool_data_a

        # DB 초기값
        try:
            db_1 = mariadb.connect(
                user = 'root',
                password = '1234',
                host = '127.0.0.1',
                port = 3306,
                database = 'test'
            )
        except mariadb.Error as e:
            print(f"Error connecting to MariaDB Platform : {e}")
            sys.exit()

        cur_1 = db_1.cursor()

        # 식별 정보
        db_number = db_1
        cur_number = cur_1
        plc_id = "plc_a"

        # run
        print("{0} {1} : starting".format(self.time_format, self.format))

        try:
            client_socket_1.connect((HOST_1, PORT_1))
            print("{0} {1} : 서버와 연결되었습니다.".format(self.time_format, self.format))
            del check_server_a[0]
            check_server_a.insert(0, 'connected')
            # request node - First step(필수적)
            send_data = binascii.a2b_hex('46494e530000000c000000000000000000000000')
            client_socket_1.sendall(send_data)
            recv_data = client_socket_1.recv(1024)
            data_0_1 = BitArray(bytes=recv_data, length=8, offset=152).bytes
            convert_0_1 = binascii.b2a_hex(data_0_1).decode()
            data_0_2 = BitArray(bytes=recv_data, length=8, offset=184).bytes
            convert_0_2 = binascii.b2a_hex(data_0_2).decode()
            print("{} {} : [Server node address(hex)] {}".format(self.time_format, self.format, convert_0_1))
            print("{} {} : [Server node address(hex)] {}".format(self.time_format, self.format, convert_0_2))
            # read data - 할당된 node를 대입하여 진행 - 동작 확인 bool 값을 받아오기 위해 데이터 길이 8 -> 9 변경
            send_data = binascii.a2b_hex('46494e530000001a000000020000000080000201' + convert_0_2 + '0001' +convert_0_1 + '00000101820000000009')

            while self.running:
                client_socket_1.sendall(send_data)
                recv_data = client_socket_1.recv(1024)
                # data diffentation
                diff_code = BitArray(bytes=recv_data, length=8, offset=232).bytes
                if diff_code == b'\x00':
                    data_1 = BitArray(bytes=recv_data, length=32, offset=240).bytes
                    convert_1 = Data_convert.data_convert(data_1)
                    data_2 = BitArray(bytes=recv_data, length=32, offset=272).bytes
                    convert_2 = Data_convert.data_convert(data_2)
                    data_3 = BitArray(bytes=recv_data, length=32, offset=304).bytes
                    convert_3 = Data_convert.data_convert(data_3)
                    data_4 = BitArray(bytes=recv_data, length=32, offset=336).bytes
                    convert_4 = Data_convert.data_convert(data_4)
                    data_5 = BitArray(bytes=recv_data, length=16, offset=368).bytes
                    convert_5 = binascii.b2a_hex(data_5).decode()
                    start_data = int(convert_5, 16)
                    if start_data == 1:
                        del bool_data_a[0]
                        bool_data_a.insert(0, 1)
                    elif start_data == 0:
                        del bool_data_a[0]
                        bool_data_a.insert(0, 0)





                    # write on db
                    data_list_1 = [db_number, cur_number, plc_id, convert_1, convert_2, convert_3, convert_4]
                    Use_db.write_db(data_list_1)

                    # read from db
                    read_key = [cur_number, plc_id]
                    read_data_1 = Use_db.read_db(read_key)

                    del datetime_a[0]
                    datetime_a.insert(9, read_data_1[0])
                    del Belt_Pos_a[0]
                    Belt_Pos_a.insert(9, read_data_1[1])
                    del Belt_Vel_a[0]
                    Belt_Vel_a.insert(9, read_data_1[2])
                    del Circle_Pos_a[0]
                    Circle_Pos_a.insert(9, read_data_1[3])
                    del Circle_Vel_a[0]
                    Circle_Vel_a.insert(9, read_data_1[4])

                    print("[{0}] {1} : Belt_Pos {2} | Belt_Vel {3} | Circle_Pos {4} | Circle_Vel {5}".format(datetime_a[9], self.format, Belt_Pos_a[9], Belt_Vel_a[9], Circle_Pos_a[9], Circle_Vel_a[9]))

                    time.sleep(1)

                elif diff_code == b'@':
                    data_5 = BitArray(bytes=recv_data, length=64, offset=240).bytes
                    convert_5 = Data_convert.data_convert_error(data_5)

                    data_6 = BitArray(bytes=recv_data, length=64, offset=304).bytes
                    convert_6 = Data_convert.data_convert_error(data_6)

                    # write on db
                    data_list_2 = [db_number, cur_number, plc_id, convert_5, convert_6]
                    Use_db.write_db_error(data_list_2)

                    # read from db
                    read_key = [cur_number, plc_id]
                    read_data_2 = Use_db.read_db(read_key)

                    print("[{0}] {1} : [EC_Error] code {2} | [MC_Error] code {3}".format(datetime_a[9], self.format, read_data_2[5], read_data_2[6]))

                    del check_server_a[0]
                    check_server_a.insert(0, 'failed')

                    if read_data_2[5] or read_data_2[6] != None:
                        EC_error_code_a.insert(0,read_data_2[5])
                        MC_error_code_a.insert(0,read_data_2[6])
                        Error_Window_PLC_A().mainloop()
                        kakao_send.refreshToken()
                        kakao_send.kakao_text()

                    break

        except TimeoutError:
            print("{0} {1} : (Error) 서버와 연결할 수 없습니다. IP주소와 포트번호를 다시 확인해주세요.".format(self.time_format, self.format))
            TimeOut_Error_Window().mainloop()
            del check_server_a[0]
            check_server_a.insert(0, 'failed')
        except ConnectionRefusedError:
            print("{0} {1} : (Error) 서버와의 연결이 거부되었습니다. 포트번호를 다시 확인해주세요.".format(self.time_format, self.format))
            ConnectionRefused_Error_Window().mainloop()
            del check_server_a[0]
            check_server_a.insert(0, 'failed')

        

        db_1.close()
        client_socket_1.close()




        # 쓰레드 종료 시 상태 변경을 위한 조건 추가 - 장두석
        del check_server_a[0]
        check_server_a.insert(0, 'failed')




        print("{0} {1} : finishing".format(self.time_format, self.format))

    def error_code(self):
        return EC_error_code_a,MC_error_code_a
    
    def resume(self):
        self.running = True

    def pause(self):
        self.running = False



class PLC_B(threading.Thread):
    def __init__(self):
        super().__init__()
        self.daemon = True
        self.running = True
        self.thread_name = 'Thread_2'
        self.plc_name = 'PLC-B'
        self.format = '(' + self.thread_name + ' ' + self.plc_name + ')'
        self.now = datetime.datetime.now()
        self.time_format = '[' + self.now.strftime('%X') + ']'
        self.running = True

    def run(self):
        # socket 초기값
        HOST_2 = '172.31.3.22'
        PORT_2 = 9600
        client_socket_2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        global check_server_b
        global bool_data_b

        # DB 초기값
        try:
            db_2 = mariadb.connect(
                user = 'root',
                password = 'jds',
                host = '127.0.0.1',
                port = 3306,
                database = 'plc_db'
            )
        except mariadb.Error as e:
            print(f"Error connecting to MariaDB Platform : {e}")
            sys.exit()

        cur_2 = db_2.cursor()

        # 식별 정보
        db_number = db_2
        cur_number = cur_2
        plc_id = "plc_b"

        # run
        print("{0} {1} : starting".format(self.time_format, self.format))

        try:
            client_socket_2.connect((HOST_2, PORT_2))
            print("{0} {1} : 서버와 연결되었습니다.".format(self.time_format, self.format))
            del check_server_b[0]
            check_server_b.insert(0, 'connected')

            # request node - First step(필수적)
            send_data = binascii.a2b_hex('46494e530000000c000000000000000000000000')

            client_socket_2.sendall(send_data)

            recv_data = client_socket_2.recv(1024)

            data_0_1 = BitArray(bytes=recv_data, length=8, offset=152).bytes
            convert_0_1 = binascii.b2a_hex(data_0_1).decode()

            data_0_2 = BitArray(bytes=recv_data, length=8, offset=184).bytes
            convert_0_2 = binascii.b2a_hex(data_0_2).decode()

            print("{} {} : [Server node address(hex)] {}".format(self.time_format, self.format, convert_0_1))
            print("{} {} : [Server node address(hex)] {}".format(self.time_format, self.format, convert_0_2))

            # read data - 할당된 node를 대입하여 진행 - 동작 확인 bool 값을 받아오기 위해 데이터 길이 8 -> 9 변경 (장두석)
            send_data = binascii.a2b_hex('46494e530000001a000000020000000080000201' + convert_0_2 + '0001' +convert_0_1 + '00000101820000000009')

            while self.running:
                client_socket_2.sendall(send_data)

                recv_data = client_socket_2.recv(1024)

                # data diffentation
                diff_code = BitArray(bytes=recv_data, length=8, offset=232).bytes

                if diff_code == b'\x00':
                    data_1 = BitArray(bytes=recv_data, length=32, offset=240).bytes
                    convert_1 = Data_convert.data_convert(data_1)

                    data_2 = BitArray(bytes=recv_data, length=32, offset=272).bytes
                    convert_2 = Data_convert.data_convert(data_2)

                    data_3 = BitArray(bytes=recv_data, length=32, offset=304).bytes
                    convert_3 = Data_convert.data_convert(data_3)

                    data_4 = BitArray(bytes=recv_data, length=32, offset=336).bytes
                    convert_4 = Data_convert.data_convert(data_4)




# 장두석 추가 시작
                    data_5 = BitArray(bytes=recv_data, length=16, offset=368).bytes
                    convert_5 = binascii.b2a_hex(data_5).decode()
                    start_data = int(convert_5, 16)
                    if start_data == 1:
                        del bool_data_b[0]
                        bool_data_b.insert(0, 1)
                    elif start_data == 0:
                        del bool_data_b[0]
                        bool_data_b.insert(0, 0)
# 장두석 추가 끝




                    # write on db
                    data_list_1 = [db_number, cur_number, plc_id, convert_1, convert_2, convert_3, convert_4]
                    Use_db.write_db(data_list_1)

                    # read from db
                    read_key = [cur_number, plc_id]
                    read_data_1 = Use_db.read_db(read_key)

                    global datetime_b, Belt_Pos_b, Belt_Vel_b, Circle_Pos_b, Circle_Vel_b

                    del datetime_b[0]
                    datetime_b.insert(9, read_data_1[0])
                    del Belt_Pos_b[0]
                    Belt_Pos_b.insert(9, read_data_1[1])
                    del Belt_Vel_b[0]
                    Belt_Vel_b.insert(9, read_data_1[2])
                    del Circle_Pos_b[0]
                    Circle_Pos_b.insert(9, read_data_1[3])
                    del Circle_Vel_b[0]
                    Circle_Vel_b.insert(9, read_data_1[4])

                    print("[{0}] {1} : Belt_Pos {2} | Belt_Vel {3} | Circle_Pos {4} | Circle_Vel {5}".format(datetime_b[9], self.format, Belt_Pos_a[9], Belt_Vel_a[9], Circle_Pos_a[9], Circle_Vel_a[9]))

                    time.sleep(1)

                elif diff_code == b'@':
                    data_5 = BitArray(bytes=recv_data, length=64, offset=240).bytes
                    convert_5 = Data_convert.data_convert_error(data_5)

                    data_6 = BitArray(bytes=recv_data, length=64, offset=304).bytes
                    convert_6 = Data_convert.data_convert_error(data_6)

                    # write on db
                    data_list_2 = [db_number, cur_number, plc_id, convert_5, convert_6]
                    Use_db.write_db_error(data_list_2)

                    # read from db
                    read_key = [cur_number, plc_id]
                    read_data_2 = Use_db.read_db(read_key)

                    print("[{0}] {1} : [EC_Error] code {2} | [MC_Error] code {3}".format(datetime_b[9], self.format, read_data_2[5], read_data_2[6]))
                    
                    del check_server_b[0]
                    check_server_b.insert(0, 'failed')
                    if read_data_2[5] or read_data_2[6] != None:
                        EC_error_code_b.insert(0,read_data_2[5])
                        MC_error_code_b.insert(0,read_data_2[6])
                        Error_Window_PLC_B().mainloop()
                        kakao_send.refreshToken()
                        kakao_send.kakao_text()
                        
                    break

        except TimeoutError:
            print("{0} {1} : (Error) 서버와 연결할 수 없습니다. IP주소와 포트번호를 다시 확인해주세요.".format(self.time_format, self.format))
            TimeOut_Error_Window().mainloop()
            del check_server_b[0]
            check_server_b.insert(0, 'failed')
        except ConnectionRefusedError:
            print("{0} {1} : (Error) 서버와의 연결이 거부되었습니다. 포트번호를 다시 확인해주세요.".format(self.time_format, self.format))
            ConnectionRefused_Error_Window().mainloop()
            del check_server_b[0]
            check_server_b.insert(0, 'failed')

        db_2.close()
        client_socket_2.close()




        # 쓰레드 종료 시 상태 변경을 위한 조건 추가 - 장두석
        del check_server_b[0]
        check_server_b.insert(0, 'failed')




        print("{0} {1} : finishing".format(self.time_format, self.format))

    def error_code(self):
        return EC_error_code_b,MC_error_code_b

    def resume(self):
        self.running = True

    def pause(self):
        self.running = False