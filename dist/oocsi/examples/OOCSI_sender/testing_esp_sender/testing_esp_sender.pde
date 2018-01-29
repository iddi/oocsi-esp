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

void setup() {
  size(200, 200);
  background(120);
  frameRate(1);

  // connect ot OOCSI server running on the same machine (localhost)
  // with "senderName" to be my channel others can send data to
  // (for more information how to run an OOCSI server refer to: https://iddi.github.io/oocsi/)
  oocsi = new OOCSI(this, "testing-ESP-receiver", "your OOCSI server address");
  oocsi.subscribe("esp-testchannel", "esptestchannel");
}

void draw() {
  // no drawing necessary
}

void esptestchannel(OOCSIEvent event) {

  println("greeting: ", event.getString("greeting", ""));
  println("count: ", event.getInt("count", 0));
  println("float_point: ", event.getFloat("float_point", 0.0));
  println("time: ", event.getLong("time", 0));
  if (event.has("intArray")) {
    float[] array = (float[]) event.getObject("intArray");
    println("intArray: ", array[0], array[1]);
  }
  if (event.has("floatArray")) {
    float[] array = (float[]) event.getObject("floatArray");
    println("floatArray: ", array[0], array[1]);
  }
} 