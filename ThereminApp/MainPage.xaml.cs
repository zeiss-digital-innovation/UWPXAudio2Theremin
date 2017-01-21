using AudioComponent;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// Die Vorlage "Leere Seite" ist unter http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 dokumentiert.

namespace ThereminApp
{
    /// <summary>
    /// Eine leere Seite, die eigenständig verwendet oder zu der innerhalb eines Rahmens navigiert werden kann.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        AudioEngine c;

        public MainPage()
        {
            this.InitializeComponent();

            c = new AudioEngine();
            Task.Run(() => c.StartEngineLoop());
        }

        ~MainPage()
        {
            c.Dispose();
        }

        private void Grid_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            SetTone(e);
            c.Start();
        }

        private void Grid_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            SetTone(e);
        }

        private void SetTone(PointerRoutedEventArgs e)
        {
            var ypos = (PlayArea.ActualHeight - e.GetCurrentPoint(this).Position.Y) / PlayArea.ActualHeight;
            var xpos = (PlayArea.ActualWidth - (PlayArea.ActualWidth - e.GetCurrentPoint(this).Position.X)) / PlayArea.ActualWidth;
            c.SetTone(ypos, xpos);
        }

        private void Grid_PointerReleased(object sender, PointerRoutedEventArgs e)
        {
            c.Stop();
        }
    }
}
