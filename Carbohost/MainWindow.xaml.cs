namespace Carbohost
{
    using Carbocontrol;
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
            this.Loaded -= this.OnLoaded;
            this.NativeHost.Child = _nativeControl;
        }
    }
}
