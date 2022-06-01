using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Runtime.InteropServices;

namespace Kirillov_lab1_sharp
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct header            
    {
        [MarshalAs(UnmanagedType.I4)]
        public int event_code;
        [MarshalAs(UnmanagedType.I4)]
        public int thread_id;
        [MarshalAs(UnmanagedType.I4)]
        public int message_size;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct confirm_header
    {
        [MarshalAs(UnmanagedType.I4)]
        public int confirm_status;
        [MarshalAs(UnmanagedType.I4)]
        public int threads_count;
    };

    public partial class Form1 : Form
    {
        // подключаем dll функции для связи с сервером

        [DllImport("FileMapping.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern bool ConnectToServer();

        [DllImport("FileMapping.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern bool SendMessageToServer(StringBuilder message, ref header h);

        [DllImport("FileMapping.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern confirm_header WaitForConfirm();

        [DllImport("sockets.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern header ReadHeader();

        [DllImport("FileMapping.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void CheckServer();

        private int count_threads = 0;
        private bool is_connected = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void RefreshListThreads(int numThreads)
        {
            listbox_threads.Items.Clear();
            count_threads = 0;
            listbox_threads.Items.Add("Все потоки");
            listbox_threads.Items.Add("Главный поток");
            for (int i = 0; i < numThreads; i++)
                listbox_threads.Items.Add($"{++count_threads}-й поток");
        }

        private void btn_connect_Click(object sender, EventArgs e)
        {
            if (!is_connected)
            {
                if (ConnectToServer())          // Подключаемся к серверу
                {
                    CheckServer();
                    confirm_header server_answer = WaitForConfirm();
                    if(server_answer.confirm_status == 1)
                    {
                        is_connected = true;

                        RefreshListThreads(server_answer.threads_count);
                    }
                    else
                    {
                        MessageBox.Show("Не удалось подключиться к серверу!");
                        return;
                    }
                }
                else 
                {
                    MessageBox.Show("Не удалось подключиться к серверу!");
                    return;
                }
            }
            else
            {
                MessageBox.Show("Вы уже подключены к серверу!");
                return;
            }
        }

        private void btn_start_Click(object sender, EventArgs e)
        {
            if (is_connected)
            {
                if (textBox_Nthreads.TextLength == 0)
                {
                    MessageBox.Show("Внимание! Задайте количество создаваемых потоков!");
                    return;
                }

                header request = new header();
                request.event_code = 0;
                request.thread_id = 0;
                int nThreads = Convert.ToInt32(textBox_Nthreads.Text);
                request.message_size = nThreads;  // В этом поле будем передавать кол-во потоков для создания

                if(!SendMessageToServer(new StringBuilder(""), ref request))
                {
                    MessageBox.Show("Не удалось отправить запрос!");
                    return;
                }
                else
                {
                    confirm_header server_answer = WaitForConfirm();
                    if(server_answer.confirm_status == 0)
                    {
                        MessageBox.Show("Сервер не обработал запрос!");
                        RefreshListThreads(server_answer.threads_count);
                        return;
                    }
                    else 
                    {
                        RefreshListThreads(server_answer.threads_count);
                    }
                }
            }
            else
            {
                MessageBox.Show("Внимание! Вы не подключены к серверу!");
                return;
            }

        }

        private void btn_stop_Click(object sender, EventArgs e)
        {
            if (is_connected)
            {
                if (count_threads > 0)
                {
                    header request = new header();
                    request.event_code = 1;
                    request.thread_id = 0;
                    request.message_size = 0;

                    if (!SendMessageToServer(new StringBuilder(""), ref request))
                    {
                        MessageBox.Show("Не удалось отправить запрос!");
                        return;
                    }
                    else
                    {
                        confirm_header server_answer = WaitForConfirm();
                        if (server_answer.confirm_status == 0)
                        {
                            MessageBox.Show("Сервер не обработал запрос!");
                            RefreshListThreads(server_answer.threads_count);
                            return;
                        }
                        else
                        {
                            RefreshListThreads(server_answer.threads_count);
                        }
                    }
                }
                else                                 
                {
                    MessageBox.Show("Внимание! Нет работающих потоков!");
                    return;
                }
            }
            else
            {
                MessageBox.Show("Внимание! Вы не подключены к серверу!");
            }
        }

        private void btn_send_Click(object sender, EventArgs e)
        {
            if (is_connected)
            {
                if (textBox_Message.TextLength == 0)
                {
                    MessageBox.Show("Внимание! Напишите текст сообщения!");
                    return;
                }
                if (listbox_threads.SelectedIndex < 0)
                {
                    MessageBox.Show("Внимание! Выберете поток!");
                    return;
                }

                StringBuilder message = new StringBuilder(textBox_Message.Text);
                header request = new header();
                request.event_code = 2;
                request.thread_id = listbox_threads.SelectedIndex - 1;
                request.message_size = message.Length;

                if (!SendMessageToServer(message, ref request))
                {
                    MessageBox.Show("Внимание! Не удалось отправить сообщение!");
                    return;
                }
                else 
                {
                    confirm_header server_answer = WaitForConfirm();
                    if (server_answer.confirm_status == 0)
                    {
                        MessageBox.Show("Сервер не обработал запрос!");
                        RefreshListThreads(server_answer.threads_count);
                        return;
                    }
                    else
                    {
                        if(message.ToString() == "quit")
                        {
                            is_connected = false;
                            listbox_threads.Items.Clear();
                            count_threads = 0;
                        }
                        else
                            RefreshListThreads(server_answer.threads_count);
                    }
                }
            }
            else
            {
                MessageBox.Show("Внимание! Вы не подключены к серверу!");
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(is_connected)
            {
                StringBuilder message = new StringBuilder("quit");
                header request = new header();
                request.event_code = 2;
                request.thread_id = 0;
                request.message_size = message.Length;
                SendMessageToServer(message, ref request);
            }
        }

    }
}
