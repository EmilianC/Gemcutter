// Copyright (c) 2017 Emilian Cioca
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace AssetManager
{
	// Loads and dispatches calls to a native-code encoder.
	class Encoder
	{
		[DllImport("kernel32", CharSet = CharSet.Ansi, SetLastError = true)]
		private static extern IntPtr LoadLibrary(string dll);

		[DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

		[DllImport("kernel32", SetLastError = true)]
		private static extern bool FreeLibrary(IntPtr hModule);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.I1)]
		private delegate bool ConvertDelegate([MarshalAs(UnmanagedType.LPStr)]string src, [MarshalAs(UnmanagedType.LPStr)]string dest);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.I1)]
		private delegate bool UpdateDelegate([MarshalAs(UnmanagedType.LPStr)]string file);

		private IntPtr dllHandle;
		private ConvertDelegate convertFunc;
		private UpdateDelegate updateFunc;

		public Encoder(string encoder)
		{
			var cd = Directory.GetCurrentDirectory();

			// We change the current directory to the target dll because it might have other adjacent dll dependencies to find.
			Directory.SetCurrentDirectory(Path.GetDirectoryName(encoder));
			dllHandle = LoadLibrary(encoder);
			Directory.SetCurrentDirectory(cd);

			if (dllHandle == IntPtr.Zero)
				throw new ArgumentException($"\"{encoder}\"\nfailed to load. Error Code {Marshal.GetLastWin32Error()}.");

			var convertHandle = GetProcAddress(dllHandle, "Convert");
			if (convertHandle == IntPtr.Zero)
				throw new ArgumentException($"{encoder}\nfailed to load the \"Convert\" function. Error code: {Marshal.GetLastWin32Error()}");

			var updateHandle = GetProcAddress(dllHandle, "Update");
			if (updateHandle == IntPtr.Zero)
				throw new ArgumentException($"{encoder}\nfailed to load the \"Update\" function. Error code: {Marshal.GetLastWin32Error()}");

			convertFunc = (ConvertDelegate)Marshal.GetDelegateForFunctionPointer(convertHandle, typeof(ConvertDelegate));
			updateFunc = (UpdateDelegate)Marshal.GetDelegateForFunctionPointer(updateHandle, typeof(UpdateDelegate));
		}

		~Encoder()
		{
			if (dllHandle != IntPtr.Zero)
				FreeLibrary(dllHandle);
		}

		public bool Convert(string file, string destination)
		{
			return convertFunc(file, destination);
		}

		public bool Update(string file)
		{
			return updateFunc(file);
		}
	}
}
