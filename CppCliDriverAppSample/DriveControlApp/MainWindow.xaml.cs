using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CppCliDll;

namespace DriveControlApp
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void OnClick_DeviceIoControl(object sender, RoutedEventArgs e)
        {
            try
            {
                var inst = new Volume();
                inst.VolumeCommand(new InData {Data1 = 3});

            }
            catch (Exception exception)
            {
                MessageBox.Show($"エラー：{exception.Message}");
            }
        }
    }
}
