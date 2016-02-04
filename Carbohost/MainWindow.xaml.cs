namespace Carbohost
{
    using Carbocontrol;
    using System;
    using System.Windows;

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly NativeControl _nativeControl;

        public MainWindow()
        {
            InitializeComponent();
            _nativeControl = new NativeControl();
            this.Loaded += this.OnLoaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            this.NativeHost.Child = _nativeControl;
            _nativeControl.MessageHook += NativeMessageFilter;
        }

        private IntPtr NativeMessageFilter(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            handled = false;
            return IntPtr.Zero;
        }
    }
}
