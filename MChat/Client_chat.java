package coms319;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Panel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Scanner;

import javax.imageio.ImageIO;
import javax.swing.AbstractListModel;
import javax.swing.BoxLayout;
import javax.swing.DefaultListCellRenderer;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.ListCellRenderer;

public class Client_chat extends JFrame{
	
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	String LoginName;
	
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					
					Client_login frame = new Client_login();
					frame.setTitle("M-Chat :: Login");
					frame.setVisible(true);
					
													
				} catch (Exception e) {
					System.out.println("Failed to initialize client connection");
					//e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the frame.
	 * @param string 
	 * @throws IOException 
	 * @throws UnknownHostException 
	 */
	public Client_chat(String uName) throws UnknownHostException, IOException {
		
		setUserLogin(uName);
		
		Socket clientSocket = new Socket("localhost", 4200);

			
		    // WAIT A WHILE FOR SERVER TO GET READY TO SEND/RECEIVE!
		    try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		    
		    PrintWriter w = new PrintWriter(clientSocket.getOutputStream());
		    w.println(uName);
		    w.flush();
		    
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(300, 100, 850, 700);
		
		JPanel basePanel = new JPanel();
		
		JPanel tPane = new JPanel();
		tPane.setLayout(new BoxLayout (tPane, BoxLayout.Y_AXIS));
	
		//Creates new panel for Message log
		
		JPanel logPane = new JPanel();
		logPane.setLayout(new BoxLayout (logPane, BoxLayout.Y_AXIS));
		
		JLabel stuff = new JLabel("Message Log:");
		stuff.setFont(new Font("Arial",Font.PLAIN,10));
		
		JList<String> cList = new JList<String>();
		ServerParseModel cListModel = new ServerParseModel();
		cList.setModel(cListModel);
		
		cList.setCellRenderer(new ListRender());
		
		JScrollPane cScroller = new JScrollPane(cList);
		cScroller.setPreferredSize(new Dimension(400,250));
		
		logPane.add(stuff);
		logPane.add(cScroller);
		
		//creates new panel for user pictures
		
		JPanel imgPane = new JPanel();
		
		JLabel stuff2 = new JLabel("Image Log:");
		stuff2.setFont(new Font("Arial",Font.PLAIN,10));
	
		ImgPanel imgPanel = new ImgPanel();
		imgPanel.setVisible(true);
		imgPane.add(stuff2);
		
		
		JList<Object> list = new JList<Object>();
		list.setCellRenderer(new ImgRender());
		ImgServerParseModel iModel = new ImgServerParseModel();
		list.setModel(iModel);
		
		JScrollPane scroller = new JScrollPane(list);
		scroller.setPreferredSize(new Dimension(200,250));
		
		imgPane.add(scroller,1);
		
		//creates new thread for client-server connection
		
		Thread cthread = new Thread((new MeassageManager(clientSocket,cListModel,imgPanel,iModel)));
		cthread.start();

		//setting up User interaction button and message fields
		
		JLabel stuff3 = new JLabel("Message:");
		stuff3.setFont(new Font("Arial",Font.PLAIN,12));
		
		JTextField message = new JTextField(null);
		
		JButton btn1 = new JButton("Send");
		btn1.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				//System.out.println(LoginName);
				//System.out.println(message.getText());
				
					String flag = "1";
					try{
						
						PrintWriter oStream = new PrintWriter(clientSocket.getOutputStream());
						oStream.println(flag);
						oStream.println(message.getText());
						oStream.flush();
						message.setText("");
						
						
					} catch (IOException e1) {
						e1.printStackTrace();
					}


			}
		});
		
		JPanel sPanel = new JPanel();
		JPanel nPanel = new JPanel();
		
		nPanel.setMaximumSize(new Dimension(300,25));		
		message.setPreferredSize( new Dimension( 200, 24 ) );
		nPanel.add(message);
		
		sPanel.setLayout(new FlowLayout());
		
		sPanel.add(stuff3);
		sPanel.add(nPanel);
		sPanel.add(btn1);

		logPane.add(sPanel);
	    //tPane.add(Box.createRigidArea(new Dimension (0, 10)));
	    
	    Client_cam cliCam = new Client_cam();
	    cliCam.setSocket(clientSocket);
	    
	    basePanel.add(logPane);
	    basePanel.add(imgPane);
		basePanel.add(tPane);
		basePanel.add(cliCam);
		
		setContentPane(basePanel);
		
	}

	public void setUserLogin(String text) {
		
		LoginName = text;
		
	}
}

class MeassageManager implements Runnable{
	
	Socket Sock;
	ServerParseModel cListModel;
	ImgPanel imgPanel = null;
	ImgServerParseModel iModel;
	
	public MeassageManager(Socket Sock, ServerParseModel cListModel,ImgPanel imgPanel,ImgServerParseModel iModel) {
		
		this.Sock = Sock;
		this.cListModel = cListModel;
		this.imgPanel = imgPanel;
		this.iModel = iModel; 
		
	}
	
	
	@Override
	public void run() {
		
		try {
			InputStream inStream = Sock.getInputStream();
			Scanner Receive =  new Scanner(inStream);
		
			while(true){
				
				String flag = Receive.nextLine();
				System.out.println("Flag :: "+flag);
				
				// when flag is one a msg is being received if 2 a picture is received
				
				if(flag.equals("0")==true){
				
					String rMessage = Receive.nextLine();
					System.out.println("Message recived : "+ rMessage);
				
					cListModel.addItem(rMessage);
					
				}else if(flag.equals("1")==true){
					
					try {
						
						String name = Receive.nextLine();
						System.out.println("name befor img "+name);
						
						/*
						*sets a byte array to receive data input
						*note: must be large in case of overrun from sync issues
						*large size allows for a read that may be an image or
						*large text left unbuffered in the stream. error handling then
						*corrects the syncing issues
						*/
						byte[] arrSize = new byte[10000];

						inStream.read(arrSize, 0, arrSize.length);				
						int s = ByteBuffer.wrap(arrSize).asIntBuffer().get();

						System.out.println("Client byte size :: " + s);
						
						byte[] arrImg = new byte[s];
						inStream.read(arrImg, 0, arrImg.length);

						BufferedImage recvImg = ImageIO.read(new ByteArrayInputStream(arrImg));

						System.out.println("Received " + recvImg.getHeight() + "x" + recvImg.getWidth() + ": "
								+ System.currentTimeMillis());

						
						
						Image image = recvImg.getScaledInstance(100, 80, Image.SCALE_SMOOTH);
						imgPanel.setImage(image);
						imgPanel.setText(name);
						
						iModel.addItem(imgPanel);
						
						//iListModel.addItem(recvImg);
						
					} catch (NegativeArraySizeException e) {
						System.out.println("negitive buff array");
						return;
					} catch (OutOfMemoryError s) {
						System.out.println("stack error");
						return;
					} catch (NullPointerException f) {
						System.out.println("Null Ptr From Client Cam");
						dumpStream(inStream);
					}

				} else {
					//clears the stream in-case of bad transfer
					
					dumpStream(inStream);
				} 
				
				dumpStream(inStream);
				
			}

		} catch (IOException e) {
			System.out.println("Warning :: Server no longer sending messages\n Close client and restart server");
			//e.printStackTrace();
		}
		
		
		
	}
	
	private void dumpStream(InputStream inStream) throws IOException{
		
		inStream.skip(inStream.available());
		
		
	}
	

	
}

class ImgPanel extends Panel {

	private static final long serialVersionUID = 1L;

	public Image myimg = null;
	private String myname ="";
	
	public Image getMyimg() {
		return myimg;
	}
	
	public void setText(String name) {
		
		this.myname = name;
		
	}
	public String getText(){
		
		return myname;
	}
	
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


class ServerParseModel extends AbstractListModel<String>{

	ArrayList<String> parserList;
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public ServerParseModel() {

		parserList = new ArrayList<String>();
		
	}
	
	@Override
	public String getElementAt(int index) {
		
		return parserList.get(index);
	}

	@Override
	public int getSize() {

		return parserList.size();
	}
	
	public void addItem (String s) {
		parserList.add(s);
		fireIntervalAdded(this,parserList.size()-1, parserList.size()-1);
	}
	
	public void delItem (int index) {
		parserList.remove(index);
		this.fireIntervalRemoved(this, parserList.size()-1, parserList.size()-1);
	}

}

class ListRender implements ListCellRenderer <Object>{

	@Override
	public Component getListCellRendererComponent(JList<? extends Object> list, Object value, int index,
			boolean isSelected, boolean cellHasFocus) {
		
		JLabel cell = new JLabel();
		cell.setText((String) value);
		cell.setOpaque(true);
		
		return cell;
	}

}

class ImgServerParseModel extends AbstractListModel<Object>{

	ArrayList<Object> parserList;
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public ImgServerParseModel() {

		parserList = new ArrayList<Object>();
		
	}
	
	@Override
	public Object getElementAt(int index) {
		
		return parserList.get(index);
	}

	@Override
	public int getSize() {

		return parserList.size();
	}
	
	public void addItem (Object s) {
		parserList.add(s);
		fireIntervalAdded(this,parserList.size()-1, parserList.size()-1);
	}
	
	public void delItem (int index) {
		parserList.remove(index);
		this.fireIntervalRemoved(this, parserList.size()-1, parserList.size()-1);
	}

}


class ImgRender extends DefaultListCellRenderer{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	@Override
	public Component getListCellRendererComponent(JList<? extends Object> list, Object value, int index,
			boolean isSelected, boolean cellHasFocus) {
		
		//Font font = new Font("helvitica", Font.BOLD, 24);
		
		JLabel label = (JLabel) super.getListCellRendererComponent(
				list, value, index, isSelected, cellHasFocus);
		
		
		ImgPanel i = ((ImgPanel) value);
		
		ImageIcon icon = new ImageIcon(i.getMyimg());
		String str = i.getText();
		
		label.setIcon(icon);
		label.setText(str);
		//label.setHorizontalTextPosition(JLabel.RIGHT);
		//label.setFont(font);
		return label;

	}

}

