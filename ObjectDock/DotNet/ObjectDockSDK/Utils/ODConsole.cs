using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;

//#pragma warning disable 649

namespace ObjectDockSDK.Utils
{
	
	/// <summary>
	/// Extended console for both Windows and Console Applications.
	/// </summary>
	public class ODConsole
	{
		#region Construction
		private ODConsole()
		{
			Initialize();
		}
		#endregion

		#region Windows API

		private struct Coord
		{
			public short X;
			public short Y;
		}

		[DllImport("user32")]
		static extern void FlashWindowEx(ref FlashWInfo info);

		[DllImport("user32")]
		static extern void MessageBeep(int type);

		[DllImport("user32")]
		static extern int SetWindowLong(IntPtr hWnd, int nIndex, int newValue);

		[DllImport("user32")]
		static extern int GetWindowLong(IntPtr hWnd, int nIndex);

		[DllImport("kernel32")]
		static extern bool AllocConsole();
		
		[DllImport("kernel32")]
		static extern bool FreeConsole();

		[DllImport("kernel32")]
		static extern bool GetConsoleScreenBufferInfo(IntPtr consoleOutput, out ConsoleScreenBufferInfo info);

		[DllImport("kernel32")]
		static extern IntPtr GetConsoleWindow();

		[DllImport("kernel32")]
		static extern IntPtr GetStdHandle(int handle);

		[DllImport("kernel32")]
		static extern int SetConsoleCursorPosition(IntPtr buffer, Coord position);
		
		[DllImport("kernel32")]
		static extern int FillConsoleOutputCharacter(IntPtr buffer, char character, int length, Coord position, out int written);

		[DllImport("kernel32")]
		static extern bool SetConsoleTextAttribute(IntPtr hConsoleOutput, ConsoleColor wAttributes);

		[DllImport("kernel32")]
		static extern bool SetConsoleTitle(string lpConsoleTitle);

		[DllImport("kernel32")]
		static extern bool SetConsoleCtrlHandler(HandlerRoutine routine, bool add);

		[DllImport("user32")]
		static extern bool ShowWindow(IntPtr hwnd, int nCmdShow);

		[DllImport("user32")]
		static extern bool IsWindowVisible(IntPtr hwnd);

		[DllImport("kernel32")]
		static extern IntPtr CreateConsoleScreenBuffer(int access, int share, IntPtr security, int flags, IntPtr reserved);

		[DllImport("kernel32")]
		static extern bool SetConsoleActiveScreenBuffer(IntPtr handle);

		[DllImport("kernel32")]
		static extern int GetConsoleCP();

		[DllImport("kernel32")]
		static extern int GetConsoleOutputCP();

		[DllImport("kernel32")]
		static extern bool SetStdHandle(int handle1, IntPtr handle2);

		[DllImport("user32")]
		static extern IntPtr GetSystemMenu(IntPtr hWnd,	bool bRevert);

		[DllImport("user32")]
		static extern bool DeleteMenu(IntPtr hMenu, int uPosition, int uFlags);

		[DllImport("user32")]
		static extern bool DrawMenuBar(IntPtr hWnd);

		#endregion
				
		#region Constants

		const int WS_MINIMIZEBOX = 0x20000;
		const int WS_MAXIMIZEBOX = 0x10000;

		const int GWL_STYLE = (-16);

		const int SW_HIDE = 0;
		const int SW_SHOW = 5;

		const int CONSOLE_TEXTMODE_BUFFER = 1;

		const int FLASHW_ALL = 3;
		const int FLASHW_TIMERNOFG = 0xc;

		const int _DefaultConsoleBufferSize = 256;

		const int GENERIC_READ = unchecked((int) 0x80000000);
		const int GENERIC_WRITE = 0x40000000;

		const int FILE_SHARE_READ = 0x1;
		const int FILE_SHARE_WRITE = 0x2;

		const int STD_OUTPUT_HANDLE = -11;
		const int STD_ERROR_HANDLE = -12;

		const int MF_BYCOMMAND = 0;
		const int SC_CLOSE = 0xF060;
		const int SC_MAXIMIZE = 0xF030;

		#endregion

		#region Structures
		private delegate bool HandlerRoutine(int type);

		/// <summary>
		/// Console Color
		/// </summary>
		[Flags]
		public enum ConsoleColor : short
		{
			/// <summary> Black </summary>
			Black		  = 0x0000,
			/// <summary> Blue </summary>
			Blue		  = 0x0001,
			/// <summary> Green </summary>
			Green		  = 0x0002,
			/// <summary> Cyan </summary>
			Cyan		  = 0x0003,
			/// <summary> Red </summary>
			Red		 	  = 0x0004,
			/// <summary> Violet </summary>
			Violet		  = 0x0005,
			/// <summary> Yellow </summary>
			Yellow		  = 0x0006,
			/// <summary> White </summary>
			White		  = 0x0007,
			/// <summary> Intensified </summary>
			Intensified	  = 0x0008,
			/// <summary> Normal (White) </summary>
			Normal	      = White,

			/// <summary> Black Background </summary>
			BlackBG		  = 0x0000,
			/// <summary> Blue Background </summary>
			BlueBG	 	  = 0x0010,
			/// <summary> Green Background </summary>
			GreenBG		  = 0x0020,
			/// <summary> Cyan Background </summary>
			CyanBG		  = 0x0030,
			/// <summary> Ted Background </summary>
			RedBG		  = 0x0040,
			/// <summary> Violet Background </summary>
			VioletBG	  = 0x0050,
			/// <summary> Yellow Background </summary>
			YellowBG      = 0x0060,
			/// <summary> White Background </summary>
			WhiteBG	      = 0x0070,
			/// <summary> Intensified Background </summary>
			IntensifiedBG = 0x0080,

			/// <summary> Underline </summary>
			Underline    = 0x4000,
			/// <summary> Reverse Video </summary>
			ReverseVideo = unchecked((short)0x8000),
		}

		private struct ConsoleScreenBufferInfo
		{
			public Coord Size;
			public Coord CursorPosition;
			public ConsoleColor Attributes;
			public SmallRect Window;
			public Coord MaximumWindowSize;
		}

		private struct SmallRect
		{
			public short Left;
			public short Top;
			public short Right;
			public short Bottom;
		}

		private struct FlashWInfo
		{
			public int Size;
			public IntPtr Hwnd;
			public int Flags;
			public int Count;
			public int Timeout;
		}
		
		#endregion

		#region Variables
		private static IntPtr buffer;
		private static bool initialized;
		#endregion

		#region Properties

		/// <summary>
		/// Specifies whether the console window should be visible or hidden
		/// </summary>
		public static bool Visible
		{
			get 
			{
				IntPtr hwnd = GetConsoleWindow();
				return hwnd != IntPtr.Zero && IsWindowVisible(hwnd);
			}
			set
			{
				Initialize();
				IntPtr hwnd = GetConsoleWindow();
				if (hwnd != IntPtr.Zero)
					ShowWindow(hwnd, value ? SW_SHOW : SW_HIDE);
			}
		}

		/// <summary>
		/// Initializes ODConsole -- should be called at the start of the program using it
		/// </summary>
		public static void Initialize()
		{
#if !LOG
			return;
#else        			
			if (initialized)
				return;

			IntPtr hwnd = GetConsoleWindow();
			initialized = true;

			// ignore CTRL+C signals
			SetConsoleCtrlHandler(null, true);
			
			// Console app
			if (hwnd != IntPtr.Zero)
			{
				buffer = GetStdHandle(STD_OUTPUT_HANDLE);
				return;
			}

			// Windows app
			bool success = AllocConsole();
			if (!success)
				return;

			buffer = CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ|FILE_SHARE_WRITE, IntPtr.Zero, CONSOLE_TEXTMODE_BUFFER, IntPtr.Zero);

			bool result = SetConsoleActiveScreenBuffer(buffer);

			SetStdHandle(STD_OUTPUT_HANDLE, buffer);
			SetStdHandle(STD_ERROR_HANDLE, buffer);

			Title = "ObjectDock Console";

			Stream s = Console.OpenStandardInput(_DefaultConsoleBufferSize);
			StreamReader reader = null;
			if (s==Stream.Null)
				reader = StreamReader.Null;
			else
				reader = new StreamReader(s, Encoding.GetEncoding(GetConsoleCP()),
					false, _DefaultConsoleBufferSize);

			Console.SetIn(reader);
    
			// Set up Console.Out
			StreamWriter writer = null;
			s = Console.OpenStandardOutput(_DefaultConsoleBufferSize);
			if (s == Stream.Null) 
				writer = StreamWriter.Null;
			else 
			{
				writer = new StreamWriter(s, Encoding.GetEncoding(GetConsoleOutputCP()),
					_DefaultConsoleBufferSize);
				writer.AutoFlush = true;
			}

			Console.SetOut(writer);

			s = Console.OpenStandardError(_DefaultConsoleBufferSize);
			if (s == Stream.Null) 
				writer = StreamWriter.Null;
			else 
			{
				writer = new StreamWriter(s, Encoding.GetEncoding(GetConsoleOutputCP()),
					_DefaultConsoleBufferSize);
				writer.AutoFlush = true;
			}
			
			Console.SetError(writer);
#endif
		}

		/// <summary>
		/// Gets or sets the title of the console window
		/// </summary>
		private static string Title
		{
			set
			{
				SetConsoleTitle(value);
			}
		}

		/// <summary>
		/// Get the current Win32 buffer handle
		/// </summary>

		private static IntPtr Buffer
		{
			get 
			{
				if (!initialized) Initialize();
				return buffer;
			}
		}

		/// <summary>
		/// Produces a simple beep.
		/// </summary>

		public static void Beep()
		{
			MessageBeep(-1);
		}

		/// <summary>
		/// Flashes the console window
		/// </summary>
		/// <param name="once">if off, flashes repeated until the user makes the console foreground</param>
		public static void Flash(bool once)
		{
			IntPtr hwnd = GetConsoleWindow();
			if (hwnd==IntPtr.Zero)
				return;

//			int style = GetWindowLong(hwnd, GWL_STYLE);
//			if ((style & WS_CAPTION)==0)
//				return;

			FlashWInfo info = new FlashWInfo();
			info.Size = Marshal.SizeOf(typeof(FlashWInfo));
			info.Flags = FLASHW_ALL;
			if (!once) info.Flags |= FLASHW_TIMERNOFG;
			FlashWindowEx(ref info);
		}

		/// <summary>
		/// Clear the console window
		/// </summary>

		public static void Clear()
		{
			Initialize();
			ConsoleScreenBufferInfo info;
			int writtenChars;
			GetConsoleScreenBufferInfo(buffer, out info);
			FillConsoleOutputCharacter(buffer, ' ', info.Size.X * info.Size.Y, new Coord(), out writtenChars);
			CursorPosition = new Coord();
		}

		/// <summary>
		/// Get the current position of the cursor
		/// </summary>
		/// 
		private static Coord CursorPosition
		{
			get	{ return Info.CursorPosition; }
			set 
			{
				Initialize();
				SetConsoleCursorPosition(buffer, new Coord());
			}
		}

		/// <summary>
		/// Redirects debug output to the console
		/// </summary>
		/// <param name="clear">clear all other listeners first</param>
		/// <param name="color">color to use for display debug output</param>
		/// <param name="beep">beep on write</param>
		public void RedirectDebugOutput(bool clear, ConsoleColor color, bool beep)
		{
			if (clear)
			{
				Debug.Listeners.Clear();
				// Debug.Listeners.Remove("Default");
			}
			Debug.Listeners.Add( new TextWriterTraceListener(new ConsoleWriter(Console.Error, color, ConsoleWriter.ConsoleFlashMode.FlashUntilResponse, beep), "console") );
		}

		/// <summary>
		/// Redirects trace output to the console
		/// </summary>
		/// <param name="clear">clear all other listeners first</param>
		/// <param name="color">color to use for display trace output</param>

		public void RedirectTraceOutput(bool clear, ConsoleColor color)
		{
			if (clear)
			{
				Trace.Listeners.Clear();
				// Trace.Listeners.Remove("Default");
			}
			Trace.Listeners.Add( new TextWriterTraceListener(new ConsoleWriter(Console.Error, color, 0, false), "console") );
		}


		/// <summary>
		/// Returns various information about the screen buffer
		/// </summary>
		static ConsoleScreenBufferInfo Info
		{
			get
			{
				ConsoleScreenBufferInfo info = new ConsoleScreenBufferInfo();
				IntPtr buffer = Buffer;
				if (buffer!=IntPtr.Zero)
					GetConsoleScreenBufferInfo(buffer, out info);
				return info;
			}
		}

		/// <summary>
		/// Gets or sets the current color and attributes of text 
		/// </summary>
		public static ConsoleColor Color
		{
			get 
			{
				return Info.Attributes;
			}
			set
			{
				IntPtr buffer = Buffer;
				if (buffer != IntPtr.Zero)
					ODConsole.SetConsoleTextAttribute(buffer, value);
			}
		}

		#endregion

	}

	internal class ConsoleWriter : TextWriter
	{

		public enum ConsoleFlashMode
		{
			NoFlashing,
			FlashOnce,
			FlashUntilResponse,
		}

		#region Variables
		TextWriter writer;
		ODConsole.ConsoleColor color;
		ConsoleFlashMode flashing;
		bool beep;
		#endregion

		#region Construction
		public ConsoleWriter(TextWriter writer, ODConsole.ConsoleColor color, ConsoleFlashMode mode, bool beep) 
		{
			this.color = color;
			this.flashing = mode;
			this.writer = writer;
			this.beep = beep;
		}
		#endregion

		#region Properties
		public ODConsole.ConsoleColor Color
		{
			get { return color; }
			set { color = value; }
		}
		
		public ConsoleFlashMode FlashMode
		{
			get { return flashing; }
			set { flashing = value; }
		}

		public bool BeepOnWrite
		{
			get { return beep; }
			set { beep = value; }
		}
		#endregion

		#region Write Routines
 
		public override Encoding Encoding
		{
			get { return writer.Encoding; }
		}

		protected void Flash()
		{
			switch (flashing)
			{
				case ConsoleFlashMode.FlashOnce:
					ODConsole.Flash(true);
					break;
				case ConsoleFlashMode.FlashUntilResponse:
					ODConsole.Flash(false);
					break;
			}

			if (beep)
				ODConsole.Beep();
		}

		public override void Write(char ch)
		{
			ODConsole.ConsoleColor oldColor = ODConsole.Color;
			try
			{
				ODConsole.Color = color;
				writer.Write(ch);
			}
			finally
			{
				ODConsole.Color = oldColor;
			}
			Flash();
		}

		public override void Write(string s)
		{
			ODConsole.ConsoleColor oldColor = ODConsole.Color;
			try
			{
				ODConsole.Color = color;
				Flash();
				writer.Write(s);
			}
			finally
			{
				ODConsole.Color = oldColor;
			}
			Flash();
		}

		public override void Write(char[] data, int start, int count)
		{
			ODConsole.ConsoleColor oldColor = ODConsole.Color;
			try
			{
				ODConsole.Color = color;
				writer.Write(data, start, count);
			}
			finally
			{
				ODConsole.Color = oldColor;
			}
			Flash();
		}
		#endregion
	}


}
