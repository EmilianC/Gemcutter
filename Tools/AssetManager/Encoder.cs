// Copyright (c) 2017 Emilian Cioca
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;

namespace AssetManager
{
	// Loads and dispatches calls to a native-code encoder.
	class Encoder
	{
		public Encoder(string encoder)
		{
			var cd = Directory.GetCurrentDirectory();

			// We change the current directory to the target dll because it might have other adjacent dll dependencies.
			Directory.SetCurrentDirectory(Path.GetDirectoryName(encoder));
			dllHandle = LoadLibrary(encoder);
			Directory.SetCurrentDirectory(cd);

			if (dllHandle == IntPtr.Zero)
				throw new ArgumentException($"\"{encoder}\"\nfailed to load. Error Code {Marshal.GetLastWin32Error()}.");

			LoadFunction(encoder, "Initialize", initializeFunc);
			LoadFunction(encoder, "Convert", convertFunc);
			LoadFunction(encoder, "Update", updateFunc);
			LoadFunction(encoder, "Validate", validateFunc);
		}

		// Helper for safely loading a function pointer from the dll.
		private void LoadFunction<T>(string encoder, string name, T funcPtr) where T : class
		{
			var funcHandle = GetProcAddress(dllHandle, name);
			if (funcHandle == IntPtr.Zero)
				throw new ArgumentException($"{encoder}\ndid not contain the \"{name}\" function. {Marshal.GetLastWin32Error()}");

			funcPtr = Marshal.GetDelegateForFunctionPointer(funcHandle, typeof(T)) as T;
		}

		~Encoder()
		{
			if (dllHandle != IntPtr.Zero)
				FreeLibrary(dllHandle);
		}

		public bool Initialize(string file)
		{
			return initializeFunc(new StringBuilder(file));
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

		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr LoadLibrary(string dll);

		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern bool FreeLibrary(IntPtr hModule);

		[DllImport("kernel32.dll", SetLastError = true)]
		private static extern int SetStdHandle(int device, IntPtr handle);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private delegate bool InitializeDelegate([In] StringBuilder file);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private delegate bool ConvertDelegate([In] StringBuilder src, [In] StringBuilder dest);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private delegate bool UpdateDelegate([In] StringBuilder file);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private delegate bool ValidateDelegate([In] StringBuilder file);

		private IntPtr dllHandle;
		private InitializeDelegate initializeFunc;
		private ConvertDelegate convertFunc;
		private UpdateDelegate updateFunc;
		private ValidateDelegate validateFunc;
	}
}
