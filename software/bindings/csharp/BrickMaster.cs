/*************************************************************
 * This file was automatically generated on 2011-09-20.      *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generator git on tinkerforge.com                   *
 *************************************************************/

namespace Tinkerforge
{
	public class BrickMaster : Device 
	{
		private static byte TYPE_GET_STACK_VOLTAGE = 1;
		private static byte TYPE_GET_STACK_CURRENT = 2;


		public BrickMaster(string uid) : base(uid) 
		{

		}

		public void GetStackVoltage(out ushort voltage)
		{
			byte[] data = new byte[4];
			LEConverter.To(stackID, 0, data);
			LEConverter.To(TYPE_GET_STACK_VOLTAGE, 1, data);
			LEConverter.To((ushort)4, 2, data);

			ipcon.Write(this, data, TYPE_GET_STACK_VOLTAGE, true);

			byte[] answer;
			if(!answerQueue.TryDequeue(out answer, IPConnection.TIMEOUT_ANSWER))
			{
				throw new TimeoutException("Did not receive answer for GetStackVoltage in time");
			}

			voltage = LEConverter.UShortFrom(4, answer);

			writeEvent.Set();
		}

		public void GetStackCurrent(out ushort current)
		{
			byte[] data = new byte[4];
			LEConverter.To(stackID, 0, data);
			LEConverter.To(TYPE_GET_STACK_CURRENT, 1, data);
			LEConverter.To((ushort)4, 2, data);

			ipcon.Write(this, data, TYPE_GET_STACK_CURRENT, true);

			byte[] answer;
			if(!answerQueue.TryDequeue(out answer, IPConnection.TIMEOUT_ANSWER))
			{
				throw new TimeoutException("Did not receive answer for GetStackCurrent in time");
			}

			current = LEConverter.UShortFrom(4, answer);

			writeEvent.Set();
		}

		public void RegisterCallback(System.Delegate d)
		{
		}
	}
}
