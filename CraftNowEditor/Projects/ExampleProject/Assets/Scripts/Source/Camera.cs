using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CraftNow;

namespace Sandbox
{
	public class Camera : Entity
	{
		void OnUpdate(float ts)
		{
			float speed = 1.0f;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.Up))
				velocity.Y = 2.0f;
			else if (Input.IsKeyDown(KeyCode.Down))
				velocity.Y = -2.0f;

			if (Input.IsKeyDown(KeyCode.Left))
				velocity.X = -2.0f;
			else if (Input.IsKeyDown(KeyCode.Right))
				velocity.X = 2.0f;

			velocity *= speed;

			Vector3 translation = Translation;
			translation += velocity * ts;
			Translation = translation;
		}

	}
}
