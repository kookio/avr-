from numpy.lib.function_base import delete
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
from tkinter import *
year = ['2020','2021','2022']
value=[100,200,500]
fig = plt.figure(figsize=(4, 4))  #그리프 그릴 창 생성
ax = fig.add_subplot(111)#창에 그래프 하나 추가
plt.bar(year,value,width=0.4)
window = Tk()  #Tk 객체 생성. 기본 윈도우 객체
window.geometry('600x600')
btu2 = Button(window,text='버튼',width=3,height=5)

label = Label(window,text='hello',width=10,height=3)
label.pack()
btu2.pack()
canvas = FigureCanvasTkAgg(fig, master=window)
canvas.draw()
canvas.get_tk_widget().pack()

def btu1():
    plt.clf()
    x=[]
    y=[]
    for i in range(5):
        x.append(i+2)
        y.append((i+2)*2*i*i*i)
            
    fig.add_subplot(111)
    plt.plot(x,y)
    print(x,y)
    canvas.draw()
    


btu = Button(window,text='버튼',width=5,height=5,command=btu1)
btu.pack()
window.mainloop()
