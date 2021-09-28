using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.Timers;
using MySql.Data.MySqlClient;
using Newtonsoft.Json.Linq;
using RestSharp;
using System.Net.Mail;
using System.Collections.Specialized;

namespace WindowsFormsApp1
{
    public partial class MainForm : Form
    {
        int count,i,j,Xlevel;
        string a, b, c, d,error1,error2;

        
        private void button2_Click(object sender, EventArgs e)
        {
            count = 1;
            Application.Exit();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            count = 1;
            pictureBox1.Image = Bitmap.FromFile("C:/Users/HP/Desktop/제목 없음.png");
        }

        public MainForm()
        {

            InitializeComponent();
        }

        

        private void BtnMessage_Click(object sender, EventArgs e)
        {
            
            Thread onethread = new Thread(Sendmsg);
            onethread.Start();
            
            if (count == 1)
                
                onethread.Abort();
        }
        private void Sendmsg()
        {
            
            TcpClient client = new TcpClient();
            MySqlConnection conn = new MySqlConnection("Server=127.0.0.1;Port=3306;Database=test;Uid=root;Pwd=1234;SSL Mode=None");
            conn.Open();
            
            client.Connect("172.31.7.11", 9601);
            if (client.Connected == true)
                Invoke(new Action(delegate ()
                {
                    pictureBox1.Image = Bitmap.FromFile("C:/Users/HP/Desktop/11.png");
                }));
            while (count==0)
            {

                byte[] buf = Encoding.Default.GetBytes("0");

                client.GetStream().Write(buf, 0, buf.Length);

                byte[] byteData = new byte[1024];
                client.GetStream().Read(byteData, 0, 2);
                byte[] byteData1 = new byte[1024];
                client.GetStream().Read(byteData1, 0, 2);
                byte[] byteData2 = new byte[8];
                client.GetStream().Read(byteData2, 0, 8);
                byte[] byteData3 = new byte[8];
                client.GetStream().Read(byteData3, 0, 8);
                i = BitConverter.ToInt32(byteData, 0);
                j = BitConverter.ToInt32(byteData1, 0);
                a = Convert.ToString(i);
                b = Convert.ToString(j);
                c = Encoding.ASCII.GetString(byteData2).Trim('\0');
                d = Encoding.ASCII.GetString(byteData3).Trim('\0');

                Invoke(new Action(delegate ()
                {
                    textBox1.Text = a;
                    textBox2.Text = b;
                    textBox4.Text = c;
                    textBox5.Text = d;

                }));
                if (textBox4.Text.Length == 0)
                {
                    error1 = "0";
                  
                }
                else
                {
                    
                    
                    error1 = textBox4.Text;
                }
                if (textBox5.Text.Length == 0)
                {
                    error2 = "0";

                }
                else
                {
                   
                   
                    error2 = textBox5.Text;
                }
                string sql = string.Format("INSERT INTO abc(A,B,C,D) VALUES ({0},{1},{2},{3})", i, j,error1, error2);
                MySqlCommand cmd = new MySqlCommand(sql, conn);
                cmd.ExecuteNonQuery();
                string xx = string.Format("현재시간 : {0}    벨트위치 : {1}   원판위치 : {2}   에러코드 :   {3}   에러코드 : {4}",System.DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"),a,b,c,d);
                
                Invoke(new Action(delegate ()
                {
                    listBox1.Items.Insert(0,xx);
                    chart1.Series[0].Points.AddXY(Xlevel, i);
                    chart1.Series[1].Points.AddXY(Xlevel, j);
                    if (chart1.Series[0].Points.Count() > 10)
                    {
                        chart1.Series[0].Points.RemoveAt(0);
                        chart1.Series[1].Points.RemoveAt(0);
                    }
                    chart1.ChartAreas[0].AxisX.Minimum = chart1.Series[0].Points[0].XValue;
                    chart1.ChartAreas[0].AxisX.Maximum = Xlevel;
                    chart1.ChartAreas[0].AxisY.Minimum = 0;
                    chart1.ChartAreas[0].AxisY.Maximum = 400;
                    chart1.ChartAreas[1].AxisX.Minimum = chart1.Series[0].Points[0].XValue;
                    chart1.ChartAreas[1].AxisX.Maximum = Xlevel;
                    chart1.ChartAreas[1].AxisY.Minimum = 0;
                    chart1.ChartAreas[1].AxisY.Maximum = 400;
                }));
                
                Thread.Sleep(1000);
                Xlevel++;
                if ((textBox4.Text.Length != 0) || (textBox5.Text.Length !=0))
                {
                    MessageBox.Show("에러가 발생하였습니다. 문제를 수정하여 다시 실행시켜주세요");
                    Invoke(new Action(delegate ()
                    {
                        pictureBox1.Image = Bitmap.FromFile("C:/Users/HP/Desktop/제목 없음.png");
                    }));
                    count = 1;
                    Sendmaill();
                    SendLine();
                    
                }
            }
            client.Close();
            

        }
       private void Sendmaill()
        {
            MailMessage message = new System.Net.Mail.MailMessage();
            message.From = new System.Net.Mail.MailAddress("keeess9999@naver.com"); //ex : ooo@naver.com
            message.To.Add("keeess9999@naver.com"); //ex : ooo@gmail.com
            message.Subject = "error code 발생";
            message.SubjectEncoding = System.Text.Encoding.UTF8;
            message.Body = c+"에러발생입니다\n"+d+"에러발생입니다";
            message.BodyEncoding = System.Text.Encoding.UTF8;
            try
            {
                System.Net.Mail.SmtpClient smtp = new System.Net.Mail.SmtpClient("smtp.naver.com", 587);
                smtp.UseDefaultCredentials = false; // 시스템에 설정된 인증 정보를 사용하지 않는다.
                smtp.EnableSsl = true;  // SSL을 사용한다.
                smtp.DeliveryMethod = System.Net.Mail.SmtpDeliveryMethod.Network; // 이걸 하지 않으면 naver 에 인증을 받지 못한다.
                smtp.Credentials = new System.Net.NetworkCredential("keeess9999@naver.com", "ZPH2LVBFQXQR");
                smtp.Send(message);
            }
            catch (System.Exception e)
            {
                MessageBox.Show(e.Message);
                MessageBox.Show("이메일 전송에 실패하였습니다");
            }
        }
        private void SendLine()
        {
            try
            {
                WebClient wc4 = new WebClient();
                string targetAddress4 = "https://notify-api.line.me/api/notify";
                wc4.Headers["Authorization"] = "Bearer LBH8xlVrSJbLi9684gt9faq9YAuE8qSfEEnbnYClY0C";

                NameValueCollection nc4 = new NameValueCollection();
                nc4["message"] = "0x"+error1 + "에러가 발생했습니다.\n" + "0x" + error2 + "에러가 발생했습니다.";

                byte[] bResult4 = wc4.UploadValues(targetAddress4, nc4);
                string result4 = Encoding.UTF8.GetString(bResult4);
            }
            catch (System.Exception e)
            {
                MessageBox.Show(e.Message);
                MessageBox.Show("라인 발송에 실패하였습니다");
            }
        }
        

       
    }

}
