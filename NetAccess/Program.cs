using System;
using System.Text;

namespace NetAccess
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			LibNetAccess net = new LibNetAccess ();
			Console.Write ("Roll No. : ");
			string roll = Console.ReadLine ();
			Console.Write ("Password : ");
			string pass = GetStringPassword();
			net.AddCredentials (roll, pass);
			if (net.Authenticate ())
				Console.WriteLine ("\nUsage : " + net.DataUsage);
			else
				Console.WriteLine ("Login error");
		}

		public static string GetStringPassword ()
		{
			StringBuilder _pwd = new StringBuilder ();

			while (true)
			{
				ConsoleKeyInfo i = Console.ReadKey (true);
				if (i.Key == ConsoleKey.Enter)
				{
					break;
				}
				if (i.Key != ConsoleKey.Backspace)
				{
					_pwd.Append (i.KeyChar);
					Console.Write ("*");
				} else if (_pwd.Length > 0)
				{
					_pwd.Remove (_pwd.Length - 1, 1);
					Console.Write ("\b \b");
				}
			}
			return _pwd.ToString ();

		}
	}
}
