"""
Thread ver_1.0.7

각 페이지별 스레딩 요소 부여

"""

## import modules
from tkinter import *
import os
from tkinter.font import Font
import threading
import datetime
import time
import sys
import numpy as np
from Sub_UI_1 import *
from Sub_UI_2 import *
from Sub_UI_3 import *
from Get_data import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

## thread
class ShowDate(threading.Thread, Tk):
    def __init__(self):
        super().__init__()
        self.daemon = True

    def run(self):
        font_day = Font(size=17, weight='bold')

        date_live = datetime.datetime.now().strftime('%Y.%m.%d')

        label = Label(font=font_day, fg='#FFFFFF', bg='#465260')
        label.place(x=1327, y=36, height=25)

        label.config(text=date_live)
        label.after(1000, self.run)

class ShowDay(threading.Thread, Tk):
    def __init__(self):
        super().__init__()
        self.daemon = True

    def run(self):
        font_day = Font(size=17, weight='bold')

        day_live = datetime.datetime.today().strftime('%A')

        label = Label(font=font_day, fg='#FFFFFF', bg='#465260')
        label.place(x=1455, y=34, height=27)

        label.config(text=day_live)
        label.after(1000, self.run)

class ShowTime(threading.Thread, Tk):
    def __init__(self):
        super().__init__()
        self.daemon = True

    def run(self):
        font_time = Font(size=45, weight='bold')

        time_live = time.strftime('%H:%M:%S')

        label = Label(font=font_time, fg='#FFFFFF', bg='#465260')
        label.place(x=1320, y=65, width=235, height=50)

        label.config(text=time_live)
        label.after(1000, self.run)

class ShowTime_test(threading.Thread, Frame):
    def __init__(self, frame):
        super().__init__()
        self.daemon = True
        self.frame = frame

    def run(self):
        font_time = Font(size=45, weight='bold')

        time_live = time.strftime('%H:%M:%S')

        label = Label(self.frame, font=font_time, fg='#FFFFFF', bg='#465260')
        label.place(x=290, y=170, height=50)

        label.config(text=time_live)
        label.after(1000, self.run)

class CheckPLC_1(threading.Thread, Frame):
    def __init__(self, frame):
        super().__init__()
        self.daemon = True
        self.frame = frame
        self.running = True

    def run(self):
        self.state_run_image_1 = PhotoImage(file=os.path.join(current_file, 'state_run_1.png'))
        self.state_run_image_2 = PhotoImage(file=os.path.join(current_file, 'state_run_2.png'))
        self.state_run = Label(self.frame, image=self.state_run_image_2, bg='#FFFFFF')
        self.state_run.place(x=325, y=325, width=100, height=50)

        self.state_idle_image_1 = PhotoImage(file=os.path.join(current_file, 'state_idle_1.png'))
        self.state_idle_image_2 = PhotoImage(file=os.path.join(current_file, 'state_idle_2.png'))
        self.state_idle = Label(self.frame, image=self.state_idle_image_2, bg='#FFFFFF')
        self.state_idle.place(x=425, y=325, width=100, height=50)

        self.state_stop_image_1 = PhotoImage(file=os.path.join(current_file, 'state_stop_1.png'))
        self.state_stop_image_2 = PhotoImage(file=os.path.join(current_file, 'state_stop_2.png'))
        self.state_stop = Label(self.frame, image=self.state_stop_image_1, bg='#FFFFFF')
        self.state_stop.place(x=525, y=325, width=100, height=50)

        # 상태 변경
        while self.running:
            if check_server_a[0] == 'failed':
                self.state_run.configure(image=self.state_run_image_2)
                self.state_idle.configure(image=self.state_idle_image_2)
                self.state_stop.configure(image=self.state_stop_image_1)

            # PLC 장비가 동작 중이 아닐 때의 조건 추가
            elif bool_data_a[0] == 0 and check_server_a[0] == 'connected':
                self.state_run.configure(image=self.state_run_image_2)
                self.state_idle.configure(image=self.state_idle_image_1)
                self.state_stop.configure(image=self.state_stop_image_2)

            elif bool_data_a[0] == 1 and check_server_a[0] == 'connected':
                self.state_run.configure(image=self.state_run_image_1)
                self.state_idle.configure(image=self.state_idle_image_2)
                self.state_stop.configure(image=self.state_stop_image_2)

            time.sleep(1)

    def resume(self):
        self.running = True

    def pause(self):

        # OFF 버튼 클릭 시 상태를 STOP으로 바꾸는 조건 추가
        self.state_run.configure(image=self.state_run_image_2)
        self.state_idle.configure(image=self.state_idle_image_2)
        self.state_stop.configure(image=self.state_stop_image_1)


        self.running = False


class CheckPLC_2(threading.Thread, Frame):
    def __init__(self, frame):
        super().__init__()
        self.daemon = True
        self.frame = frame
        self.running = True

    def run(self):
        self.state_run_image_1 = PhotoImage(file=os.path.join(current_file, 'state_run_1.png'))
        self.state_run_image_2 = PhotoImage(file=os.path.join(current_file, 'state_run_2.png'))
        self.state_run = Label(self.frame, image=self.state_run_image_2, bg='#FFFFFF')
        self.state_run.place(x=980, y=325, width=100, height=50)

        self.state_idle_image_1 = PhotoImage(file=os.path.join(current_file, 'state_idle_1.png'))
        self.state_idle_image_2 = PhotoImage(file=os.path.join(current_file, 'state_idle_2.png'))
        self.state_idle = Label(self.frame, image=self.state_idle_image_2, bg='#FFFFFF')
        self.state_idle.place(x=1080, y=325, width=100, height=50)

        self.state_stop_image_1 = PhotoImage(file=os.path.join(current_file, 'state_stop_1.png'))
        self.state_stop_image_2 = PhotoImage(file=os.path.join(current_file, 'state_stop_2.png'))
        self.state_stop = Label(self.frame, image=self.state_stop_image_1, bg='#FFFFFF')
        self.state_stop.place(x=1180, y=325, width=100, height=50)

        # 상태 변경
        while self.running:
            if check_server_b[0] == 'failed':
                self.state_run.configure(image=self.state_run_image_2)
                self.state_idle.configure(image=self.state_idle_image_2)
                self.state_stop.configure(image=self.state_stop_image_1)

            elif bool_data_b[0] == 0 and check_server_b[0] == 'connected':
                self.state_run.configure(image=self.state_run_image_2)
                self.state_idle.configure(image=self.state_idle_image_1)
                self.state_stop.configure(image=self.state_stop_image_2)



# 장두석 추가 시작
            elif bool_data_b[0] == 1 and check_server_b[0] == 'connected':
                self.state_run.configure(image=self.state_run_image_1)
                self.state_idle.configure(image=self.state_idle_image_2)
                self.state_stop.configure(image=self.state_stop_image_2)
# 장두석 추가 끝



            time.sleep(1)
        print("ch2 finish")

    def resume(self):
        self.running = True

    def pause(self):


        # OFF 버튼 클릭 시 상태를 STOP으로 바꾸는 조건 추가 - 장두석
        self.state_run.configure(image=self.state_run_image_2)
        self.state_idle.configure(image=self.state_idle_image_2)
        self.state_stop.configure(image=self.state_stop_image_1)
        

        self.running = False

class Update_1(threading.Thread, Frame):
    def __init__(self, frame):
        super().__init__()
        self.daemon = True
        self.frame = frame
        self.running = True

    def run(self):
        # print
        data_1_1_entry = Entry(self.frame, justify='right')
        data_1_1_entry.place(x=175, y=433, width=110, height=22)

        data_1_2_entry = Entry(self.frame, justify='right')
        data_1_2_entry.place(x=175, y=463, width=110, height=22)

        data_1_3_entry = Entry(self.frame, justify='right')
        data_1_3_entry.place(x=480, y=433, width=110, height=22)

        data_1_4_entry = Entry(self.frame, justify='right')
        data_1_4_entry.place(x=480, y=463, width=110, height=22)

        # draw
        i = 0
        fig_1 = plt.figure()
        x_1 = [0 for i in range(0, 10)]
        y_1 = [0 for i in range(0, 10)]

        fig_2 = plt.figure()
        x_2 = [0 for i in range(0, 10)]
        y_2 = [0 for i in range(0, 10)]
        #막대그래프 표현 
        label = ['true','false','all']
        x_3 = np.arange(3)
        y_3 = [0,0,0]
        
        y_4 = [0,0,0]
        y_5 = [0,0,0]
        y_6 = [0,0,0]
        y_7 = [0, 0, 0]
        width = 0.35
       
        

        canvas_1 = FigureCanvasTkAgg(fig_1, master=self.frame)
        canvas_1.get_tk_widget().place(x=35, y=495, width=300, height=220)

        canvas_2 = FigureCanvasTkAgg(fig_2, master=self.frame)
        canvas_2.get_tk_widget().place(x=340, y=495, width=300, height=220)

        while self.running:
            data_1_1_entry.delete(0, END)
            data_1_1_entry.insert(0, Belt_Pos_a[9])
            data_1_1_entry.update()

            data_1_2_entry.delete(0, END)
            data_1_2_entry.insert(0, Belt_Vel_a[9])
            data_1_2_entry.update()

            data_1_3_entry.delete(0, END)
            data_1_3_entry.insert(0, Circle_Pos_a[9])
            data_1_3_entry.update()

            data_1_4_entry.delete(0, END)
            data_1_4_entry.insert(0, Circle_Vel_a[9])
            data_1_4_entry.update()

            fig_1.clear()
            ax_1 = fig_1.add_subplot(1, 1, 1)
            ax_1.set(ylim=[0, 400], title='Position_Belt')
            ax_1.plot(x_1, y_1, color='blue')

            fig_2.clear()
            ax_2 = plt.subplot(1, 1, 1)
            # ax_2.plot(x_2, y_2, color='red')

            #막대그래프 그려주는 코드
            ax_2.bar(label,y_3, width, color='red',label='true')
            ax_2.bar(label,y_4, width, color='blue')
            ax_2.bar(label , y_5, width, yerr=y_7 ,color='red')
            ax_2.bar(label , y_6, width, bottom=y_5,color='blue')
            
            
            
            canvas_1.draw()
            canvas_1.get_tk_widget().update()

            canvas_2.draw()
            canvas_2.get_tk_widget().update()

            new_x_1 = x_1[9] + 1
            del x_1[0]
            x_1.insert(9, new_x_1)
            del y_1[0]
            y_1.insert(9, Belt_Pos_a[9])

            # new_x_2 = x_2[9] + 1
            # del x_2[0]
            # x_2.insert(9, new_x_2)
            # del y_2[0]
            # y_2.insert(9, Circle_Pos_a[9])
            
            
            new_y_3 = y_3[0] + 1
            new_y_4 = y_4[1] + 2
            del y_3[0]
            del y_4[1]
            del y_5[2]
            del y_6[2]
            y_3.insert(0, new_y_3)
            y_4.insert(1, new_y_4)
            y_5.insert(2, new_y_3)
            y_6.insert(2, new_y_4)


            # new_y_5 = y_3[2] + 10
            # del y_3[2]
            # y_3.insert(2, new_y_5)
            
            time.sleep(1)
        print("up1 finish")

    def resume(self):
        self.running = True

    def pause(self):
        self.running = False

class Update_2(threading.Thread, Frame):
    def __init__(self, frame):
        super().__init__()
        self.daemon = True
        self.frame = frame
        self.running = True

    def run(self):
        # print
        data_1_1_entry = Entry(self.frame, justify='right')
        data_1_1_entry.place(x=830, y=433, width=110, height=22)

        data_1_2_entry = Entry(self.frame, justify='right')
        data_1_2_entry.place(x=830, y=463, width=110, height=22)

        data_1_3_entry = Entry(self.frame, justify='right')
        data_1_3_entry.place(x=1135, y=433, width=110, height=22)

        data_1_4_entry = Entry(self.frame, justify='right')
        data_1_4_entry.place(x=1135, y=463, width=110, height=22)

        # draw
        fig_1 = plt.figure()
        x_1 = [0 for i in range(0, 10)]
        y_1 = [0 for i in range(0, 10)]

        fig_2 = plt.figure()
        x_2 = [0 for i in range(0, 10)]
        y_2 = [0 for i in range(0, 10)]

        canvas_1 = FigureCanvasTkAgg(fig_1, master=self.frame)
        canvas_1.get_tk_widget().place(x=690, y=495, width=300, height=220)

        canvas_2 = FigureCanvasTkAgg(fig_2, master=self.frame)
        canvas_2.get_tk_widget().place(x=995, y=495, width=300, height=220)

        while self.running:
            data_1_1_entry.delete(0, END)
            data_1_1_entry.insert(0, Belt_Pos_b[9])
            data_1_1_entry.update()

            data_1_2_entry.delete(0, END)
            data_1_2_entry.insert(0, Belt_Vel_b[9])
            data_1_2_entry.update()

            data_1_3_entry.delete(0, END)
            data_1_3_entry.insert(0, Circle_Pos_b[9])
            data_1_3_entry.update()

            data_1_4_entry.delete(0, END)
            data_1_4_entry.insert(0, Circle_Vel_b[9])
            data_1_4_entry.update()

            fig_1.clear()
            ax_1 = fig_1.add_subplot(1, 1, 1)
            ax_1.set(ylim=[0, 400], title='Position_Belt')
            ax_1.plot(x_1, y_1, color='blue')

            fig_2.clear()
            ax_2 = fig_2.add_subplot(1, 1, 1)
            ax_2.set(ylim=[0, 400], title='Position_Circle')
            ax_2.plot(x_2, y_2, color='red')

            canvas_1.draw()
            canvas_1.get_tk_widget().update()

            canvas_2.draw()
            canvas_2.get_tk_widget().update()

            new_x_1 = x_1[9] + 1
            del x_1[0]
            x_1.insert(9, new_x_1)
            del y_1[0]
            y_1.insert(9, Belt_Pos_b[9])

            new_x_2 = x_2[9] + 1
            del x_2[0]
            x_2.insert(9, new_x_2)
            del y_2[0]
            y_2.insert(9, Circle_Pos_b[9])

            time.sleep(1)
        print("ch2 finish")

    def resume(self):
        self.running = True

    def pause(self):
        self.running = False