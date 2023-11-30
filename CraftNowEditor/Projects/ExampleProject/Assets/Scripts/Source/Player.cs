using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using CraftNow;

namespace Sandbox
{
	public class Player : Entity
	{
		private TransformComponent m_Transform;
		private Rigidbody2DComponent m_Rigidbody;
		private bool jump_flag;

		public float Speed;
		public float Time = 0.0f;

		void OnCreate()
		{
			Console.WriteLine($"Script: Player.OnCreate - {ID}");

			m_Transform = GetComponent<TransformComponent>();
			m_Rigidbody = GetComponent<Rigidbody2DComponent>();

			jump_flag = false;
		}

		void OnUpdate(float ts)
		{
			// Console.WriteLine($"Player.OnUpdate: {ts}");
			Time += ts;

			float speed = 0.01f;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.Space))
			{
				if (!jump_flag)
				{
					velocity.Y = 30.0f;
					Console.WriteLine($"Up");
					jump_flag = true;
				}
			
			}
			else if (Input.IsKeyDown(KeyCode.S))
				velocity.Y = -1.0f;
			else
				jump_flag = false;

			if (Input.IsKeyDown(KeyCode.A))
				velocity.X = -1.0f;
			else if (Input.IsKeyDown(KeyCode.D))
				velocity.X = 1.0f;

			Entity cameraEntity = FindEntityByName("相机");
			if (cameraEntity != null)
			{
				Camera camera = cameraEntity.As<Camera>();

				if (Input.IsKeyDown(KeyCode.Q))
				{
					camera.DistanceFromPlayer += speed * 2.0f;
					Console.WriteLine("Q press");
				}
				else if (Input.IsKeyDown(KeyCode.E))
				{
					camera.DistanceFromPlayer -= speed * 2.0f;
					Console.WriteLine("E press");
				}
			}

			velocity *= speed;

			m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);

			//Vector3 translation = m_Transform.Translation;
			//translation += velocity * ts;
			//m_Transform.Translation = translation;
		}

	}
}

namespace Zelda
{
	public class Player : Entity
	{
		private TransformComponent m_Transform;
		private Rigidbody2DComponent m_Rigidbody;

		public float Speed;
		public float Time = 0.0f;

		void OnCreate()
		{
			Console.WriteLine($"Script: Player.OnCreate - {ID}");

			m_Transform = GetComponent<TransformComponent>();
			m_Rigidbody = GetComponent<Rigidbody2DComponent>();
		}

		void OnUpdate(float ts)
		{
			// Console.WriteLine($"Player.OnUpdate: {ts}");
			Time += ts;

			float speed = 0.01f;
			Vector3 velocity = Vector3.Zero;

			//Vector3 translation = Translation;

			if (Input.IsKeyDown(KeyCode.W))
			{
				velocity.Y = 2.0f;
				//translation.Y += 2.0f;
				Console.WriteLine("W");
			}
			else if (Input.IsKeyDown(KeyCode.S))
			{
				velocity.Y = -2.0f;
				//translation.Y -= 2.0f;
				Console.WriteLine("S");
			}

			if (Input.IsKeyDown(KeyCode.A))
			{
				velocity.X = -2.0f;
				//translation.X -= 1.0f;
				Console.WriteLine("A");
			}
			else if (Input.IsKeyDown(KeyCode.D))
			{
				velocity.X = 2.0f;
				//translation.X += 1.0f;
				Console.WriteLine("D");
			}

			Entity cameraEntity = FindEntityByName("Camera");
			if (cameraEntity != null)
			{
				Camera camera = cameraEntity.As<Camera>();

				if (Input.IsKeyDown(KeyCode.Q))
				{
					camera.DistanceFromPlayer += speed * 2.0f;
					Console.WriteLine("Q press");
				}
				else if (Input.IsKeyDown(KeyCode.E))
				{
					camera.DistanceFromPlayer -= speed * 2.0f;
					Console.WriteLine("E press");
				}
			}

			velocity *= speed;

			//m_Rigidbody.ApplyLinearImpulse(velocity.XZ, true);

			Vector3 translation = Translation;
			translation += velocity;
			Translation = translation;
		}

	}
}
