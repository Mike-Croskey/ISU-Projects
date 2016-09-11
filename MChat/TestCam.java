/**
 * 
 */
package coms319;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.FileOutputStream;
import javax.media.*;
import javax.media.control.FrameGrabbingControl;
import javax.media.format.VideoFormat;
import javax.media.util.BufferToImage;
import javax.swing.*;
public class TestCam extends Panel implements ActionListener {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public static Player player = null;
	public CaptureDeviceInfo di = null;
	public MediaLocator ml = null;
	public JButton capture = null;
	public Buffer buf = null;
	public Image img = null;

	public BufferToImage btoi = null;
	public ImagePanel imgpanel = null;
	
	public TestCam() {
		setLayout(new BorderLayout());
		setSize(320,550);
		imgpanel = new ImagePanel();
		capture = new JButton("Capture");
		capture.addActionListener(this);
		//String str1 = "vfw//:0";
		String str2 = "vfw:Microsoft WDM Image Capture (Win32):0";
		//Getting Camera Device Information
		di = CaptureDeviceManager.getDevice(str2);
		ml = di.getLocator(); //MediaLocator for Camera
		try {
			player = Manager.createRealizedPlayer(ml);//Creating Player
			player.start(); //Start Camera Player
			Component comp; //for Getting Visual Player Component of Camera
			if ((comp = player.getVisualComponent()) != null) {
				add(comp,BorderLayout.NORTH); //Add Camera to North
			}
			add(capture,BorderLayout.CENTER); //Add Capture Button in Between
			//add(imgpanel,BorderLayout.SOUTH); //Add Captured image
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void main(String[] args) {
		Frame f = new Frame("SwingCapture"); //Creating New Frame
		TestCam cf = new TestCam(); //Main Object Create
		//Window Closing
		f.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				playerclose(); //Closing Player
				System.exit(0);
			}
		});
		f.add("Center",cf); //Adding Main Object to Frame
		f.pack(); //Packing Frame
		f.setSize(new Dimension(320,550));
		f.setVisible(true);
	}
	public static void playerclose()//Function While Closing Player 
	{
		player.close();
		player.deallocate();
	}
	public void actionPerformed(ActionEvent e) {
		JComponent c = (JComponent) e.getSource();//Getting Source of Event
		if (c == capture)//Capture Button Event 
		{
			FrameGrabbingControl fgc = (FrameGrabbingControl)
			player.getControl("javax.media.control.FrameGrabbingControl");
			buf = fgc.grabFrame(); // Grabbing a frame
			// Convert it to an image Buffer
			btoi = new BufferToImage((VideoFormat)buf.getFormat());
			img = btoi.createImage(buf); //Creating Image from Buffer
			imgpanel.setImage(img); // show the image
			saveJPG(img,"/test.jpg"); // save image Function
		}
	}
	class ImagePanel extends Panel //Panel for Displaying Cptured Image
	{
		public Image myimg = null;
		public ImagePanel() {
			setLayout(null);
			setSize(220,140);
		}
		public void setImage(Image img) {
			this.myimg = img;
			repaint();
		}
		public void paint(Graphics g) {
			if (myimg != null) {
				g.drawImage(myimg, 0, 0, this);
			}
		}
	}
	public static void saveJPG(Image img, String s) {
		BufferedImage bi = new BufferedImage(img.getWidth(null),
		img.getHeight(null), BufferedImage.TYPE_INT_RGB);
		Graphics2D g2 = bi.createGraphics();
		g2.drawImage(img, null, null);
		FileOutputStream out = null;
		try {
			out = new FileOutputStream(s);
		} catch (java.io.FileNotFoundException io) {
			System.out.println("File Not Found");
		}
		try {
			out.close();
		} catch (java.io.IOException io) {
			System.out.println("IOException");
		}
	}
}