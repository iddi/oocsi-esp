import nl.tue.id.oocsi.*;

// ******************************************************
// This example requires a running OOCSI server!
//
// How to do that? Check: Examples > Tools > LocalServer
//
// More information how to run an OOCSI server
// can be found here: https://iddi.github.io/oocsi/)
// ******************************************************

OOCSI oocsi;
String channelNames[] = new String[] {"esp-testchannel1", "esp-testchannel2", "esp-testchannel3"}; 

void setup() {
  size(200, 200);
  background(120);
  frameRate(1);

  // connect ot OOCSI server running on the same machine (localhost)
  // with "senderName" to be my channel others can send data to
  // (for more information how to run an OOCSI server refer to: https://iddi.github.io/oocsi/)
  oocsi = new OOCSI(this, "testing-ESP-sender", "your server address");
}

void draw() {
  // select random channel
  int selection = round(random(0, 2));

  // send to OOCSI ...
  oocsi
    // on the randomly selected channel...
    .channel(channelNames[selection])
    // add different types of data
    .data("greeting", "hello esp-testchannel1")
    .data("float_point", (float) sin(frameCount/10.))
    .data("count", frameCount)
    .data("time", millis())
    .data("intArray", new int[] {1, 2, 3})
    .data("floatArray", new float[] {1.3, 4.5})
    // send finally
    .send();
}