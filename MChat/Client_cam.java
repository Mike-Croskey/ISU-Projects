/**
 * 
 */
package coms319;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Panel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.nio.ByteBuffer;

import javax.imageio.ImageIO;
import javax.media.Buffer;
import javax.media.CaptureDeviceInfo;
import javax.media.CaptureDeviceManager;
import javax.media.Manager;
import javax.media.MediaLocator;
import javax.media.Player;
import javax.media.control.FrameGrabbingControl;
import javax.media.format.VideoFormat;
import javax.media.util.BufferToImage;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;



/**
 * @author GoFYSPlus
 *
 */
public class Client_cam extends Panel {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public static Player camPlay = null;
	public CaptureDeviceInfo capDev = null;
	public MediaLocator locator = null;
	public Buffer buf = null;
	public Image img = null;
	public BufferToImage bufToImg = null;
	public ImgPanel imgPanel = null;
	public JButton getCamData = null;
	
	
	private Socket client = null;
	
	public void setSocket(Socket s){
		
		client = s;
		
	}
	
	public static void main(String[] args) {
		
		Frame base = new Frame("Client_Cam");
		Client_cam clientCam = new Client_cam(); 
		
		base.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				camPlay.close(); //Closing Player
				System.exit(0);
			}
		});
		
		base.add("Center",clientCam); //Adding Main Object to Frame
		base.setVisible(true);
	}
	
	
	public Client_cam(){
		
		imgPanel = new ImgPanel();
		
		this.setLayout(new BorderLayout());
		this.setSize(50,50);
		getCamData = new JButton("Send Picture");
		getCamData.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				
					FrameGrabbingControl fgc = (FrameGrabbingControl)camPlay.getControl("javax.media.control.FrameGrabbingControl");
					buf = fgc.grabFrame(); 
					bufToImg = new BufferToImage((VideoFormat)buf.getFormat());
					img = bufToImg.createImage(buf); 
					
			 
					
					Image image = img.getScaledInstance(100, 80, Image.SCALE_SMOOTH);
					imgPanel.setImage(image);
					
					try {
						BufferedImage buffered = new BufferedImage(img.getWidth(null), img.getHeight(null), BufferedImage.TYPE_INT_ARGB);
						buffered.getGraphics().drawImage(img, 0, 0 , null);
			        
						ByteArrayOutputStream byteArrOSTR = new ByteArrayOutputStream();
						String flag = "2";
					
					
						OutputStream outStream = client.getOutputStream();
						PrintWriter pWrite = new PrintWriter(outStream);

						
						ImageIO.write(buffered,"jpg",byteArrOSTR);
						byte[] buffSize = ByteBuffer.allocate(10000).putInt(byteArrOSTR.size()).array();
						
						int s = ByteBuffer.wrap(buffSize).asIntBuffer().get();
						System.out.println("BuffSize :: "+s);
						
						pWrite.println(flag);
						pWrite.flush();
						
						outStream.write(buffSize);
						outStream.flush();
						outStream.write(byteArrOSTR.toByteArray());
						outStream.flush();
						
						
						
						pWrite.flush();
						
						buffered.getGraphics().dispose();
						
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					} catch (NullPointerException e2){
						e2.printStackTrace();
					}
						
				
					
				
			}
		});
		
		String camId = "vfw:Microsoft WDM Image Capture (Win32):0";
		
		capDev = CaptureDeviceManager.getDevice(camId);
		locator = capDev.getLocator();
		try{
			camPlay = Manager.createRealizedPlayer(locator);
			camPlay.start();
			
			Component camFeed;
			camFeed = camPlay.getVisualComponent();
			
			JPanel imgGroupPanel = new JPanel();
			JLabel Screenshot = new JLabel("Last Image Sent:");
			
			imgGroupPanel.add(Screenshot);
			imgGroupPanel.add(imgPanel);
			
			
			add(camFeed,BorderLayout.NORTH);
			add(imgGroupPanel,BorderLayout.SOUTH);
			add(getCamData,BorderLayout.CENTER);
			
			
		}catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void close() {
		
		camPlay.close();
		camPlay.deallocate();
		
	}
	
	private class ImgPanel extends Panel {

		private static final long serialVersionUID = 1L;

		public Image myimg = null;
		public ImgPanel() {
			setLayout(null);
			setSize(100,80);
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
	
}
