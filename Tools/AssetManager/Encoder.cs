// Copyright (c) 2017 Emilian Cioca
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace AssetManager
{
	class Encoder
	{
		public Encoder(string encoder)
		{
			dllHandle = LoadLibrary(encoder);

			if (dllHandle == IntPtr.Zero)
				throw new ArgumentException($"\"{encoder}\"\nfailed to load.");

			var funcHandle = GetProcAddress(dllHandle, "Reset");
			if (funcHandle != IntPtr.Zero)
				resetFunc = (ResetDelegate)Marshal.GetDelegateForFunctionPointer(funcHandle, typeof(ResetDelegate));
			else
				throw new ArgumentException($"\"{encoder}\"\ndid not contain the \"Reset\" function.");

			funcHandle = GetProcAddress(dllHandle, "Convert");
			if (funcHandle != IntPtr.Zero)
				convertFunc = (ConvertDelegate)Marshal.GetDelegateForFunctionPointer(funcHandle, typeof(ConvertDelegate));
			else
				throw new ArgumentException($"\"{encoder}\"\ndid not contain the \"Convert\" function.");

			funcHandle = GetProcAddress(dllHandle, "Update");
			if (funcHandle != IntPtr.Zero)
				updateFunc = (UpdateDelegate)Marshal.GetDelegateForFunctionPointer(funcHandle, typeof(UpdateDelegate));
			else
				throw new ArgumentException($"\"{encoder}\"\ndid not contain the \"Update\" function.");

			funcHandle = GetProcAddress(dllHandle, "Validate");
			if (funcHandle != IntPtr.Zero)
				validateFunc = (ValidateDelegate)Marshal.GetDelegateForFunctionPointer(funcHandle, typeof(ValidateDelegate));
			else
				throw new ArgumentException($"\"{encoder}\"\ndid not contain the \"Validate\" function.");
		}

		~Encoder()
		{
			if (dllHandle != IntPtr.Zero)
				FreeLibrary(dllHandle);
		}

		public bool Reset(string file)
		{
			return resetFunc(new StringBuilder(file));
		}

		public bool Convert(string file, string destination)
		{
			return convertFunc(new StringBuilder(file), new StringBuilder(destination));
		}

		public bool Update(string file)
		{
			return updateFunc(new StringBuilder(file));
		}

		public bool ValidateMetaData(string file)
		{
			return validateFunc(new StringBuilder(file));
		}

		[DllImport("kernel32.dll")]
		private static extern IntPtr LoadLibrary(string dll);

		[DllImport("kernel32.dll")]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

		[DllImport("kernel32.dll")]
		private static extern bool FreeLibrary(IntPtr hModule);

		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern int SetStdHandle(int device, IntPtr handle);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		delegate bool ResetDelegate([In] StringBuilder file);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		delegate bool ConvertDelegate([In] StringBuilder src, [In] StringBuilder dest);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		delegate bool UpdateDelegate([In] StringBuilder file);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		delegate bool ValidateDelegate([In] StringBuilder file);

		private IntPtr dllHandle;
		private ResetDelegate resetFunc;
		private ConvertDelegate convertFunc;
		private UpdateDelegate updateFunc;
		private ValidateDelegate validateFunc;
	}
}
