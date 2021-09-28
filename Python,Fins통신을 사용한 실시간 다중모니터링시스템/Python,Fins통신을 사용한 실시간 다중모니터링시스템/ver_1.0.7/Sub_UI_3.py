"""
Sub_UI_3 ver_1.0.7

데이터 표시 페이지 요소 구현
전체적인 코드 수정 (내부 frame을 시각적 표현만 하도록)

"""

## import modules
from tkinter import *
import os
from tkinter.font import Font
import sys
from Thread import *

## initial value
# file direction
current_file = os.path.dirname(__file__)

# color
color_main_bg = '#465260'
color_frame_bg = '#3A4552'
color_data_bg = '#3C4048'

## sequence
class SubFrame_3(Frame):
    def __init__(self):
        super().__init__(width=1330, height=750)
        # font
        self.font_25 = Font(size=25, weight='bold')
        self.font_20 = Font(size=20, weight='bold')
        self.font_15 = Font(size=15, weight='bold')
        self.font_19 = Font(size=19, weight='bold')
        self.font_12 = Font(size=12, weight='bold')

        # 구성요소 불러오기
        self.initUI()

    def initUI(self):
        # frame
        frame_0 = Label(self, background=color_main_bg)
        frame_0.place(x=0, y=0, width=1330, height=750)

        frame_1 = Label(self, background=color_frame_bg)
        frame_1.place(x=20, y=20, width=635, height=710)

        frame_2 = Label(self, background=color_frame_bg)
        frame_2.place(x=675, y=20, width=635, height=710)

        # frame_1 component
        """machine info"""
        machine_name_1 = Label(self, text='PLC_E', font=self.font_25, fg='#FFFFFF', bg=color_frame_bg)
        machine_name_1.place(x=20, y=20, width=635, height=60)

        self.machine_image_1 = PhotoImage(file=os.path.join(current_file, 'machine_image_sample.png'))
        machine_image_1 = Label(self, image=self.machine_image_1, bg='#FFFFFF')
        machine_image_1.place(x=35, y=80, width=250, height=300)

        """comment"""
        comment_frame_1 = Label(self, background=color_frame_bg)
        comment_frame_1.place(x=300, y=80, width=340, height=140)

        comment_title_1 = Label(self, text='Comment', font=self.font_20, fg='#FFFFFF', bg=color_frame_bg)
        comment_title_1.place(x=305, y=80)

        comment_1 = Label(self, text='This is sample.\nYou can see CAM mechanism.\nLine\nLine', font=self.font_15, justify='left',
                            fg='#FFFFFF', bg=color_frame_bg)
        comment_1.place(x=305, y=115)

        """ON/OFF"""
        run_title_1 = Label(self, text='Monitoring', font=self.font_20, fg='#FFFFFF', bg=color_frame_bg)
        run_title_1.place(x=305, y=225)

        self.run_btn_bg_image_1 = PhotoImage(file=os.path.join(current_file, 'button_background.png'))
        run_button_bg_1 = Label(self, image=self.run_btn_bg_image_1, bg=color_frame_bg)
        run_button_bg_1.place(x=465, y=220, width=170, height=50)

        self.run_btn_on_image_1_1 = PhotoImage(file=os.path.join(current_file, 'button_on_1.png'))
        self.run_btn_on_image_1_2 = PhotoImage(file=os.path.join(current_file, 'button_on_2.png'))
        self.run_button_on_1 = Button(self, image=self.run_btn_on_image_1_2, bg='#FFFFFF', bd=0, command=self.btn_cmd_1)
        self.run_button_on_1.place(x=470, y=225, width=80, height=40)

        self.run_btn_off_image_1_1 = PhotoImage(file=os.path.join(current_file, 'button_off_1.png'))
        self.run_btn_off_image_1_2 = PhotoImage(file=os.path.join(current_file, 'button_off_2.png'))
        self.run_button_off_1 = Button(self, image=self.run_btn_off_image_1_1, bg='#FFFFFF', bd=0)
        self.run_button_off_1.place(x=550, y=225, width=80, height=40)

        """state"""
        state_title_1 = Label(self, text='State', font=self.font_20, fg='#FFFFFF', bg=color_frame_bg)
        state_title_1.place(x=305, y=285)

        self.state_bg_image_1 = PhotoImage(file=os.path.join(current_file, 'state_background.png'))
        state_bg_1 = Label(self, image=self.state_bg_image_1, bg=color_frame_bg)
        state_bg_1.place(x=320, y=325, width=310, height=50)

        self.state_run_image_1 = PhotoImage(file=os.path.join(current_file, 'state_run_2.png'))
        state_run_1 = Label(self, image=self.state_run_image_1, bg='#FFFFFF')
        state_run_1.place(x=325, y=325, width=100, height=50)

        self.state_idle_image_1 = PhotoImage(file=os.path.join(current_file, 'state_idle_2.png'))
        state_idle_1 = Label(self, image=self.state_idle_image_1, bg='#FFFFFF')
        state_idle_1.place(x=425, y=325, width=100, height=50)

        self.state_stop_image_1 = PhotoImage(file=os.path.join(current_file, 'state_stop_1.png'))
        state_stop_1 = Label(self, image=self.state_stop_image_1, bg='#FFFFFF')
        state_stop_1.place(x=525, y=325, width=100, height=50)

        """data"""
        data_1_frame = Label(self, background=color_data_bg)
        data_1_frame.place(x=35, y=395, width=605, height=320)

        date_1_title = Label(self, text='Data', font=self.font_19, fg='#FFFFFF', background=color_data_bg)
        date_1_title.place(x=40, y=395)

        data_1_1_name = Label(self, text='Belf-Pos', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_1_1_name.place(x=45, y=430)

        data_1_2_name = Label(self, text='Belf-Vel', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_1_2_name.place(x=45, y=460)

        data_1_3_name = Label(self, text='Circle-Pos', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_1_3_name.place(x=345, y=430)

        data_1_4_name = Label(self, text='Circle-Vel', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_1_4_name.place(x=345, y=460)

        data_1_1_entry = Entry(self, justify='right')
        data_1_1_entry.place(x=175, y=433, width=110, height=22)

        data_1_2_entry = Entry(self, justify='right')
        data_1_2_entry.place(x=175, y=463, width=110, height=22)

        data_1_3_entry = Entry(self, justify='right')
        data_1_3_entry.place(x=480, y=433, width=110, height=22)

        data_1_4_entry = Entry(self, justify='right')
        data_1_4_entry.place(x=480, y=463, width=110, height=22)

        data_1_1_unit = Label(self, text='mm', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_1_1_unit.place(x=285, y=433)

        data_1_2_unit = Label(self, text='mm/s', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_1_2_unit.place(x=285, y=463)

        data_1_3_unit = Label(self, text='mm', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_1_3_unit.place(x=590, y=433)

        data_1_4_unit = Label(self, text='mm/s', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_1_4_unit.place(x=590, y=463)

        """graph"""
        canvas_1_1 = Canvas(self)
        canvas_1_1.place(x=35, y=495, width=300, height=220)

        canvas_1_2 = Canvas(self)
        canvas_1_2.place(x=340, y=495, width=300, height=220)

        # frame_2 component
        """machine info"""
        machine_name_2 = Label(self, text='PLC_F', font=self.font_25, fg='#FFFFFF', bg=color_frame_bg)
        machine_name_2.place(x=675, y=20, width=635, height=60)

        self.machine_image_2 = PhotoImage(file=os.path.join(current_file, 'machine_image_sample.png'))
        machine_image_2 = Label(self, image=self.machine_image_2, bg='#FFFFFF')
        machine_image_2.place(x=690, y=80, width=250, height=300)

        """comment"""
        comment_frame_2 = Label(self, background=color_frame_bg)
        comment_frame_2.place(x=955, y=80, width=340, height=140)

        comment_title_2 = Label(self, text='Comment', font=self.font_20, fg='#FFFFFF', bg=color_frame_bg)
        comment_title_2.place(x=960, y=80)

        comment_2 = Label(self, text='This is sample.\nYou can see CAM mechanism.\nLine\nLine', font=self.font_15, justify='left',
                            fg='#FFFFFF', bg=color_frame_bg)
        comment_2.place(x=960, y=115)

        """ON/OFF"""
        run_title_2 = Label(self, text='Monitoring', font=self.font_20, fg='#FFFFFF', bg=color_frame_bg)
        run_title_2.place(x=960, y=225)

        self.run_btn_bg_image_2 = PhotoImage(file=os.path.join(current_file, 'button_background.png'))
        run_button_bg_2 = Label(self, image=self.run_btn_bg_image_2, bg=color_frame_bg)
        run_button_bg_2.place(x=1120, y=220, width=170, height=50)

        self.run_btn_on_image_2_1 = PhotoImage(file=os.path.join(current_file, 'button_on_1.png'))
        self.run_btn_on_image_2_2 = PhotoImage(file=os.path.join(current_file, 'button_on_2.png'))
        self.run_button_on_2 = Button(self, image=self.run_btn_on_image_2_2, bg='#FFFFFF', bd=0)
        self.run_button_on_2.place(x=1125, y=225, width=80, height=40)

        self.run_btn_off_image_2_1 = PhotoImage(file=os.path.join(current_file, 'button_off_1.png'))
        self.run_btn_off_image_2_2 = PhotoImage(file=os.path.join(current_file, 'button_off_2.png'))
        self.run_button_off_2 = Button(self, image=self.run_btn_off_image_2_1, bg='#FFFFFF', bd=0)
        self.run_button_off_2.place(x=1205, y=225, width=80, height=40)

        """state"""
        state_title_2 = Label(self, text='State', font=self.font_20, fg='#FFFFFF', bg=color_frame_bg)
        state_title_2.place(x=960, y=285)

        self.state_bg_image_2 = PhotoImage(file=os.path.join(current_file, 'state_background.png'))
        state_bg_2 = Label(self, image=self.state_bg_image_2, bg=color_frame_bg)
        state_bg_2.place(x=975, y=325, width=310, height=50)

        self.state_run_image_2 = PhotoImage(file=os.path.join(current_file, 'state_run_2.png'))
        state_run_2 = Label(self, image=self.state_run_image_2, bg='#FFFFFF')
        state_run_2.place(x=980, y=325, width=100, height=50)

        self.state_idle_image_2 = PhotoImage(file=os.path.join(current_file, 'state_idle_2.png'))
        state_idle_2 = Label(self, image=self.state_idle_image_2, bg='#FFFFFF')
        state_idle_2.place(x=1080, y=325, width=100, height=50)

        self.state_stop_image_2 = PhotoImage(file=os.path.join(current_file, 'state_stop_1.png'))
        state_stop_2 = Label(self, image=self.state_stop_image_2, bg='#FFFFFF')
        state_stop_2.place(x=1180, y=325, width=100, height=50)

        """data"""
        data_2_frame = Label(self, background=color_data_bg)
        data_2_frame.place(x=690, y=395, width=605, height=320)

        date_2_title = Label(self, text='Data', font=self.font_19, fg='#FFFFFF', background=color_data_bg)
        date_2_title.place(x=695, y=395)

        data_2_1_name = Label(self, text='Belf-Pos', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_2_1_name.place(x=700, y=430)

        data_2_2_name = Label(self, text='Belf-Vel', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_2_2_name.place(x=700, y=460)

        data_2_3_name = Label(self, text='Circle-Pos', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_2_3_name.place(x=1000, y=430)

        data_2_4_name = Label(self, text='Circle-Vel', font=self.font_15, fg='#FFFFFF', bg=color_data_bg)
        data_2_4_name.place(x=1000, y=460)

        data_2_1_entry = Entry(self, justify='right')
        data_2_1_entry.place(x=830, y=433, width=110, height=22)

        data_2_2_entry = Entry(self, justify='right')
        data_2_2_entry.place(x=830, y=463, width=110, height=22)

        data_2_3_entry = Entry(self, justify='right')
        data_2_3_entry.place(x=1135, y=433, width=110, height=22)

        data_2_4_entry = Entry(self, justify='right')
        data_2_4_entry.place(x=1135, y=463, width=110, height=22)

        data_2_1_unit = Label(self, text='mm', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_2_1_unit.place(x=940, y=433)

        data_2_2_unit = Label(self, text='mm/s', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_2_2_unit.place(x=940, y=463)

        data_2_3_unit = Label(self, text='mm', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_2_3_unit.place(x=1245, y=433)

        data_2_4_unit = Label(self, text='mm/s', font=self.font_12, fg='#FFFFFF', bg=color_data_bg)
        data_2_4_unit.place(x=1245, y=463)

        """graph"""
        canvas_1_1 = Canvas(self)
        canvas_1_1.place(x=690, y=495, width=300, height=220)

        canvas_1_2 = Canvas(self)
        canvas_1_2.place(x=995, y=495, width=300, height=220)

    def btn_cmd_1(self):
        pass
    #     # worker 실행
    #     self.worker_1.start()
    #     self.worker_2.start()

    #     self.run_button_on_1.configure(image=self.run_btn_on_image_1_1)
    #     self.run_button_off_1.configure(image=self.run_btn_off_image_1_2)