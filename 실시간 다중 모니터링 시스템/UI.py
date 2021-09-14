"""
UI ver_1.0.7

UI 업데이트 v2

페이지 전환의 개념을 notebook을 이용
전체적인 코드 수정

notebook을 이용할 때 내부 frame을 가진 frame은 적용이 안됨.
    -> frame을 시각적인 표현만 하기로 함.

off를 누르면 모니터링 스레딩 종료

"""

## import modules
from tkinter import *
from tkinter.ttk import *
import os
from tkinter.font import Font
import sys
from Sub_UI_1 import *
from Sub_UI_2 import *
from Sub_UI_3 import *
from Thread import *
from winsound import *
import Get_data

## initial value
# file direction
current_file = os.path.dirname(__file__)

# color
color_main_bg = '#465260'
color_frame_bg = '#3A4552'
color_tab_bg = '#3A4757'
color_btn_normal = '#364153'
color_btn_clicked = '#2A313E'

## sub_section_0
class Subframe_0(Frame):
    def __init__(self):
        super().__init__(width=1330, height=750)
        self.initUI()

    def initUI(self):
        # frame
        frame_0 = Label(self, background=color_main_bg)
        frame_0.place(x=0, y=0, width=1330, height=750)

        frame_1 = Label(self, background=color_frame_bg)
        frame_1.place(x=20, y=20, width=1290, height=710)

## main section
class MainWindow(Tk):
    def __init__(self):
        super().__init__()

        # window size (16:9 비율)
        w = 1600    # 창 너비
        h = 900     # 창 높이
        sw = self.winfo_screenwidth()   # 스크린 너비
        sh = self.winfo_screenheight()  # 스크린 높이
        x = int((sw - w) / 2)   # 창 생성 시 x 좌표
        y = int((sh - h) / 2)   # 창 생성 시 y 좌표

        # window format
        self.title('Factory Monitoring')    # 프로그램 타이틀
        self.geometry('{0}x{1}+{2}+{3}'.format(w, h, x, y))    # 창 생성 조건 설정
        self.resizable(False, False)    # 창 사이즈 고정
        self.iconbitmap(os.path.join(current_file, 'icon.ico'))

        # font
        self.font_version = Font(size=12)
        self.font_main_title_1 = Font(size=45, weight='bold')
        self.font_main_title_2 = Font(size=30, weight='bold')
        self.font_tab_button = Font(size=18, weight='bold')

        # worker 생성
        self.worker_1 = ShowDate()
        self.worker_2 = ShowDay()
        self.worker_3 = ShowTime()

        # 구성요소 불러오기
        self.initUI()

    def initUI(self):
        # notebook
        self.notebook = Notebook(self, width=1330, height=750)  # notebook 생성
        self.notebook.place(x=269, y=127)

        page_0 = Subframe_0()
        self.notebook.add(page_0)

        page_1 = SubFrame_1()
        self.notebook.add(page_1)

        page_2 = SubFrame_2()
        self.notebook.add(page_2)

        page_3 = SubFrame_3()
        self.notebook.add(page_3)

        # title
        frame_title = Frame(self, background=color_main_bg)    # title
        frame_title.place(x=0, y=0, width=1600, height=150)

        self.title_image = PhotoImage(file=os.path.join(current_file, 'main_title_60px.png'))
        title_label_1 = Label(frame_title, image=self.title_image, bg=color_main_bg)
        title_label_1.place(x=430, y=40, width=60, height=60)

        title_label_2 = Label(frame_title, text='KOPO Factory', font=self.font_main_title_1, fg='#FFFFFF', bg=color_main_bg)
        title_label_2.place(x=500, y=35)

        title_label_3 = Label(frame_title, text='[Seongnam]', font=self.font_main_title_2, fg='#FFFFFF', bg=color_main_bg)
        title_label_3.place(x=915, y=50)

        title_label_4 = Label(frame_title, text='Made by AI+X\nVer 1.0.7', font=self.font_version, fg='#7F7F7F', bg=color_main_bg, justify='left')
        title_label_4.place(x=20, y=95)

        division_line = Label(frame_title, background='#7F7F7F')    # 구분선
        division_line.place(x=0, y=145, width=1600, height=5)

        self.worker_1.start()    # 날짜 표시
        self.worker_2.start()    # 요일 표시
        self.worker_3.start()    # 시간 표시

        # side tab menu
        frame_tab = Frame(self, background=color_tab_bg)
        frame_tab.place(x=0, y=150, width=270, height=750)

        self.tab_button_1 = Button(frame_tab, text='H O M E', font=self.font_tab_button, fg='#FFFFFF', bg=color_btn_clicked, bd=0,
                                    activeforeground='#FFFFFF', activebackground=color_btn_clicked, command=self.btn_cmd_1)
        self.tab_button_1.place(x=0, y=30, width=270, height=50)

        self.tab_button_2 = Button(frame_tab, text='Section1', font=self.font_tab_button, fg='#FFFFFF', bg=color_btn_normal, bd=0,
                                    activeforeground='#FFFFFF', activebackground=color_btn_clicked, command=self.btn_cmd_2)
        self.tab_button_2.place(x=0, y=80, width=270, height=50)

        self.tab_button_3 = Button(frame_tab, text='Section2', font=self.font_tab_button, fg='#FFFFFF', bg=color_btn_normal, bd=0,
                                    activeforeground='#FFFFFF', activebackground=color_btn_clicked, command=self.btn_cmd_3)
        self.tab_button_3.place(x=0, y=130, width=270, height=50)

        self.tab_button_4 = Button(frame_tab, text='Section3', font=self.font_tab_button, fg='#FFFFFF', bg=color_btn_normal, bd=0,
                                    activeforeground='#FFFFFF', activebackground=color_btn_clicked, command=self.btn_cmd_4)
        self.tab_button_4.place(x=0, y=180, width=270, height=50)

    def btn_cmd_1(self):
        self.tab_button_1.configure(bg=color_btn_clicked)
        self.tab_button_2.configure(bg=color_btn_normal)
        self.tab_button_3.configure(bg=color_btn_normal)
        self.tab_button_4.configure(bg=color_btn_normal)
        self.notebook.select(0)

    def btn_cmd_2(self):
        self.tab_button_1.configure(bg=color_btn_normal)
        self.tab_button_2.configure(bg=color_btn_clicked)
        self.tab_button_3.configure(bg=color_btn_normal)
        self.tab_button_4.configure(bg=color_btn_normal)
        self.notebook.select(1)

    def btn_cmd_3(self):
        self.tab_button_1.configure(bg=color_btn_normal)
        self.tab_button_2.configure(bg=color_btn_normal)
        self.tab_button_3.configure(bg=color_btn_clicked)
        self.tab_button_4.configure(bg=color_btn_normal)
        self.notebook.select(2)

    def btn_cmd_4(self):
        self.tab_button_1.configure(bg=color_btn_normal)
        self.tab_button_2.configure(bg=color_btn_normal)
        self.tab_button_3.configure(bg=color_btn_normal)
        self.tab_button_4.configure(bg=color_btn_clicked)
        self.notebook.select(3)
#PLC_A의 에러발생이벤트
class Error_Window_PLC_A(Tk):
    def __init__(self):
        super().__init__()
        get_error_code = Get_data.PLC_A()
        error_code = get_error_code.error_code()
        self.title('MOTER_ERROR')
        self.geometry('500x300+800+500')
        self.configure(background='#465260')
        self.iconbitmap(bitmap = 'C:/Users/HP\Documents/카카오톡 받은 파일/ver_1.0.7+에러메세지추가+run상태추가+카카오톡 메시지발송/ver_1.0.7+에러메세지추가/ver_1.0.7/warning_105171.ico')
        PlaySound('Windows Ding.wav',SND_FILENAME)
        self.Error_image = PhotoImage(file=os.path.join(current_file, 'warning_105171.png'),master=self)
        error_image = Label(self, image=self.Error_image,bg='#465260')
        error_image.place(x=10,y=85)
        error_msg= '에러가 발생하였습니다 \n\n EC_Error_Code : {0} \n\nMC_Error_Code : {1}\n\n장비를 재시작 해주세요'.format((error_code[0]),error_code[1])
        error_text = Label(self,text=error_msg,bg='#465260',fg = "White",font=('Lucida Grande',18))
        error_text.place(x=90,y=30)
        self.Error_image2 = PhotoImage(file=os.path.join(current_file, 'button_background3.png'),master=self)
        error_label = Label(self,image=self.Error_image2,bd=0,bg='#465260')
        error_label.place(x=175,y=215)
        self.Error_image3 = PhotoImage(file=os.path.join(current_file, 'Error_button.png'),master=self)
        error_button = Button(self,image=self.Error_image3,bd=0,bg='white',command=self.error_btu_cmd)
        error_button.place(x=180,y=220)
    def error_btu_cmd(self):
        self.destroy()

#PLC_B의 에러발생이벤트
class Error_Window_PLC_B(Tk):
    def __init__(self):
        super().__init__()
        get_error_code = Get_data.PLC_B()
        error_code = get_error_code.error_code()
        self.title('MOTER_ERROR')
        self.geometry('500x300+800+500')
        self.configure(background='#465260')
        self.iconbitmap(bitmap = 'C:/Users/HP\Documents/카카오톡 받은 파일/ver_1.0.7+에러메세지추가+run상태추가+카카오톡 메시지발송/ver_1.0.7+에러메세지추가/ver_1.0.7/warning_105171.ico')
        PlaySound('Windows Ding.wav',SND_FILENAME)
        self.Error_image = PhotoImage(file=os.path.join(current_file, 'warning_105171.png'),master=self)
        error_image = Label(self, image=self.Error_image,bg='#465260')
        error_image.place(x=10,y=85)
        error_msg= '에러가 발생하였습니다 \n\n EC_Error_Code : {0} \n\nMC_Error_Code : {1}\n\n장비를 재시작 해주세요'.format((error_code[0]),error_code[1])
        error_text = Label(self,text=error_msg,bg='#465260',fg = "White",font=('Lucida Grande',18))
        error_text.place(x=90,y=30)
        self.Error_image2 = PhotoImage(file=os.path.join(current_file, 'button_background3.png'),master=self)
        error_label = Label(self,image=self.Error_image2,bd=0,bg='#465260')
        error_label.place(x=175,y=215)
        self.Error_image3 = PhotoImage(file=os.path.join(current_file, 'Error_button.png'),master=self)
        error_button = Button(self,image=self.Error_image3,bd=0,bg='white',command=self.error_btu_cmd)
        error_button.place(x=180,y=220)
    def error_btu_cmd(self):
        self.destroy()
#소켓연결 타임아웃이벤트
class TimeOut_Error_Window(Tk):
    def __init__(self):
        super().__init__()
        self.title('TIMEOUT_ERROR')
        self.geometry('500x200+800+500')
        self.configure(background='#465260')
        self.iconbitmap(bitmap = 'C:/Users/HP\Documents/카카오톡 받은 파일/ver_1.0.7+에러메세지추가+run상태추가+카카오톡 메시지발송/ver_1.0.7+에러메세지추가/ver_1.0.7/warning_105171.ico')
        PlaySound('Windows Ding.wav',SND_FILENAME)
        self.Error_image = PhotoImage(file=os.path.join(current_file, 'warning_105171.png'),master=self)
        error_image = Label(self, image=self.Error_image,bg='#465260')
        error_image.place(x=10,y=35)
        error_text = Label(self,text='(Error) 서버와 연결할 수 없습니다.\n\nIP주소와 포트번호를 다시 확인해주세요.',bg='#465260',fg = "White",font=('Lucida Grande',15))
        error_text.place(x=90,y=30)
        self.Error_image2 = PhotoImage(file=os.path.join(current_file, 'button_background3.png'),master=self)
        error_label = Label(self,image=self.Error_image2,bd=0,bg='#465260')
        error_label.place(x=175,y=115)
        self.Error_image3 = PhotoImage(file=os.path.join(current_file, 'Error_button.png'),master=self)
        error_button = Button(self,image=self.Error_image3,bd=0,bg='white',command=self.error_btu_cmd)
        error_button.place(x=180,y=120)
    def error_btu_cmd(self):
        self.destroy()
#소켓연결 포트연결이상 이벤트
class ConnectionRefused_Error_Window(Tk):
    def __init__(self):
        super().__init__()
        self.title('CONNECTION_ERROR')
        self.geometry('500x200+800+500')
        self.configure(background='#465260')
        self.iconbitmap(bitmap = 'C:/Users/HP\Documents/카카오톡 받은 파일/ver_1.0.7+에러메세지추가+run상태추가+카카오톡 메시지발송/ver_1.0.7+에러메세지추가/ver_1.0.7/warning_105171.ico')
        PlaySound('Windows Ding.wav',SND_FILENAME)
        self.Error_image = PhotoImage(file=os.path.join(current_file, 'warning_105171.png'),master=self)
        error_image = Label(self, image=self.Error_image,bg='#465260')
        error_image.place(x=10,y=35)
        error_text = Label(self,text='(Error) 서버와의 연결이 거부되었습니다.\n\n포트번호를 다시 확인해주세요.',bg='#465260',fg = "White",font=('Lucida Grande',15))
        error_text.place(x=90,y=30)
        self.Error_image2 = PhotoImage(file=os.path.join(current_file, 'button_background3.png'),master=self)
        error_label = Label(self,image=self.Error_image2,bd=0,bg='#465260')
        error_label.place(x=175,y=115)
        self.Error_image3 = PhotoImage(file=os.path.join(current_file, 'Error_button.png'),master=self)
        error_button = Button(self,image=self.Error_image3,bd=0,bg='white',command=self.error_btu_cmd)
        error_button.place(x=180,y=120)
    def error_btu_cmd(self):
        self.destroy()


def main():
    app = MainWindow()
    app.mainloop()

if __name__ == '__main__':
    main()