Đây là bài tập cuối khoá khi mình học khoá học lập trình STM32 tại TAPIT.

# Đề

Viết chương trình mô phỏng hoạt động của mạch điều khiển đèn giao thông theo các mô tả sau đây:

LED giao thông có 3 đèn: XANH, VÀNG, ĐỎ với hai chế độ hoạt động như sau: 
Chế độ 1: Đèn VÀNG nhấp nháy 1s 1 lần (1 giây sáng, 1 giây tắt), đèn XANH và đèn ĐỎ tắt. Thời gian nháy 1s được tính từ ngoại vi TIMER. 
Chế độ 2: Đèn XANH, VÀNG, ĐỎ sáng luân phiên với thời gian mặc định ban đầu là 05s-03s-05s, có thể thay đổi thời gian. 

**CHỨC NĂNG CẤU HÌNH THỜI GIAN**
Cấu hình thời gian đèn sáng qua giao tiếp UART với cú pháp sau: CFGTIMExxyyzz\r
Với xx là thời gian sáng của đèn XANH, yy là thời gian sáng của đèn VÀNG, zz là thời gian sáng của đèn ĐỎ, thời gian có 2 chữ số, đơn vị giây.  CFGTIME: là báo hiệu gói tin cấu hình. \r là kí tự kết thúc câu báo hiệu hết lệnh cấu hình. Thời gian cấu hình từ hợp lệ: 
Đèn XANH tối thiểu 05s - tối đa 45 giây
Đèn Vàng tối thiểu 03 giây - tối đa 05 giây
Đèn ĐỎ tối thiểu 05s - tối đa 90 giây
Sau khi nhận gói tin, thiết bị phản hồi lại gói tin cấu hình đã gửi xuống và: 
Nếu thời gian cấu hình hợp lệ, thiết bị phản hồi cấu hình thành công: CFGTIME OK\r
Nếu thời gian cấu hình không hợp lệ, thiết bị phản hồi cấu hình thất bại CFGTIME FAIL\r
Tại thiết bị, thời gian sáng của các đèn được tính từ TIMER

**CHỨC NĂNG THAY ĐỔI CHẾ ĐỘ**
Cho phép cấu hình thay đổi chế độ hoạt động bằng một nút nhấn sử dụng EXTI và UART, mặc định khi thiết bị khởi động thì sẽ hoạt động với chế độ 1.

**Cấu hình qua nút nhấn**: Nếu nhấn nút thì thiết bị sẽ thay đổi chế độ. Nếu đang ở chế độ 1 thì sang chế độ 2 và ngược lại. Khi chuyển đổi chế độ, cần truyền một thông báo qua UART với cú pháp: MODEx SELECTED với x là 1 hoặc 2 tùy chế độ được chọn. 

**Cấu hình qua UART**: Nếu cấu hình bằng UART thì sử dụng cú pháp CFGMODEx\r để chọn chế độ 1 (x=1) hoặc chế độ 2(x=2). 
Sau khi nhận gói tin: 
Thiết bị phản hồi lại gói tin cấu hình đã nhận được
Nếu x hợp lệ (1 hoặc 2) thì thiết bị phản hồi cấu hình thành công: MODEx SELECTED với x là 1 hoặc 2 tùy chế độ được chọn. 
Nếu MODE không hợp lệ thì thiết bị phản hồi cấu hình thất bại: CFGMODE FAIL\r. 
Nếu nhận dữ liệu không hợp hệ từ UART không thuộc 2 cấu hình về thời gian và chế độ thì báo ERROR\r. 

# Giải

https://drive.google.com/drive/folders/1PLTDTN1qOWYV2mg_PuHGrZnVvcqirteZ

Cấu hình trên CubeMx: [ioc](traffic_light.pdf)

Code trong file main

Kết quả: [text](<Kết quả.docx>)