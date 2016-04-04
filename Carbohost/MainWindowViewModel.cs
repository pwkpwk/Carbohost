namespace Carbohost
{
    using System;
    using System.ComponentModel;
    using System.Windows;
    using System.Windows.Input;
    sealed class MainWindowViewModel : INotifyPropertyChanged
    {
        private readonly Command _showFloater;
        private bool _isOtherPanelVisible;

        private sealed class Command : ICommand
        {
            public event EventHandler CanExecuteChanged;

            bool ICommand.CanExecute(object parameter)
            {
                return true;
            }

            void ICommand.Execute(object parameter)
            {
                Window floater = new Floater();
                floater.Show();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public MainWindowViewModel()
        {
            _showFloater = new Command();
            _isOtherPanelVisible = false;
        }

        public bool IsOtherPanelVisible
        {
            get { return _isOtherPanelVisible; }
            set
            {
                if(value != _isOtherPanelVisible)
                {
                    _isOtherPanelVisible = value;
                    this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsOtherPanelVisible)));
                }
            }
        }

        public ICommand ShowFloater
        {
            get { return _showFloater; }
        }
    }
}
