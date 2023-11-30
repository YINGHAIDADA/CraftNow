namespace CraftNow
{
	public class Input
	{
		public static bool IsKeyDown(KeyCode keycode)
		{
			return InternalCalls.Input_IsKeyDown(keycode);
		}

		public static bool IsKeyRepeat(KeyCode keycode)
		{
			return InternalCalls.Input_IsKeyRepeat(keycode);
		}
	}
}
