package com.tencent.kuikly.demo.pages.compose

import androidx.compose.runtime.Composable
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.key
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import com.tencent.kuikly.compose.ComposeContainer
import com.tencent.kuikly.compose.coil3.rememberAsyncImagePainter
import com.tencent.kuikly.compose.foundation.Image
import com.tencent.kuikly.compose.foundation.background
import com.tencent.kuikly.compose.foundation.clickable
import com.tencent.kuikly.compose.foundation.gestures.detectTapGestures
import com.tencent.kuikly.compose.foundation.layout.Arrangement
import com.tencent.kuikly.compose.foundation.layout.Box
import com.tencent.kuikly.compose.foundation.layout.Column
import com.tencent.kuikly.compose.foundation.layout.PaddingValues
import com.tencent.kuikly.compose.foundation.layout.Row
import com.tencent.kuikly.compose.foundation.layout.Spacer
import com.tencent.kuikly.compose.foundation.layout.fillMaxSize
import com.tencent.kuikly.compose.foundation.layout.fillMaxWidth
import com.tencent.kuikly.compose.foundation.layout.height
import com.tencent.kuikly.compose.foundation.layout.padding
import com.tencent.kuikly.compose.foundation.layout.size
import com.tencent.kuikly.compose.foundation.layout.width
import com.tencent.kuikly.compose.foundation.lazy.LazyColumn
import com.tencent.kuikly.compose.foundation.shape.RoundedCornerShape
import com.tencent.kuikly.compose.material3.Text
import com.tencent.kuikly.compose.resources.painterResource
import com.tencent.kuikly.compose.setContent
import com.tencent.kuikly.compose.ui.Alignment
import com.tencent.kuikly.compose.ui.Modifier
import com.tencent.kuikly.compose.ui.draw.clip
import com.tencent.kuikly.compose.ui.geometry.Offset
import com.tencent.kuikly.compose.ui.geometry.Size
import com.tencent.kuikly.compose.ui.graphics.Color
import com.tencent.kuikly.compose.ui.input.pointer.pointerInput
import com.tencent.kuikly.compose.ui.text.font.FontWeight
import com.tencent.kuikly.compose.ui.unit.dp
import com.tencent.kuikly.compose.ui.unit.sp
import com.tencent.kuikly.core.annotations.Page
import com.tencent.kuikly.core.datetime.DateTime
import com.tencent.kuikly.core.log.KLog

@Page("LazyColumnJankDemo")
internal class LazyColumnJankDemo : ComposeContainer() {
    override fun willInit() {
        super.willInit()
        setContent {
            var items = remember { mutableStateListOf<String>() }
            for (i in 0..500) {
                items.add("$i")
            }
            LazyColumn(beyondBoundsItemCount = 5) {
                items(items.size) { index ->
                    item("index:$index, value:${items[index]}", Color.Red, index)
                }
            }
        }
    }

    @Composable
    private fun item(text: String, backgroundColor: Color, index: Int) {
        var uri by remember { mutableStateOf("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png") }

        Column(modifier = Modifier.clickable {
            KLog.i("KCMPListDemo", "click at index:$index")
        }) {
            Text(text, fontSize = 16.sp, color = Color.Black)
            Row(
                horizontalArrangement = Arrangement.SpaceEvenly, // 水平方向剩余空间均匀分配
                verticalAlignment = Alignment.CenterVertically,  // 垂直方向居中
                modifier = Modifier.fillMaxWidth(1f).height(100.dp)
            ) {
                var backgroundColor: Color = Color.Green
                if (index % 2 == 0) {
                    backgroundColor = Color.Yellow
                }

                if (index % 2 == 0) {
                    // View组件demo
                    Column(
                        modifier = Modifier.size(80.dp, 50.dp).background(color = backgroundColor)
                    ) {
                        Text("这是KCMView")
                    }
                    // Image组件demo
                    Image(
                        contentDescription = null,
                        painter = rememberAsyncImagePainter(uri),
                        modifier = Modifier.size(50.dp, 50.dp)
                            // 圆角demo
                            .clip(RoundedCornerShape(topStart = 16.dp, bottomEnd = 16.dp))
                    )
                    Image(
                        contentDescription = null,
                        painter = rememberAsyncImagePainter(uri),
                        modifier = Modifier.size(50.dp, 50.dp)
                            // 圆角demo
                            .clip(RoundedCornerShape(topStart = 16.dp, bottomEnd = 16.dp))
                    )
                    Image(
                        contentDescription = null,
                        painter = rememberAsyncImagePainter(uri),
                        modifier = Modifier.size(50.dp, 50.dp)
                            // 圆角demo
                            .clip(RoundedCornerShape(topStart = 16.dp, bottomEnd = 16.dp))
                    )
                } else {
                    Image(
                        contentDescription = null,
                        painter = rememberAsyncImagePainter(uri),
                        modifier = Modifier.size(50.dp, 50.dp)
                            // 圆角demo
                            .clip(RoundedCornerShape(topStart = 16.dp, bottomEnd = 16.dp))
                    )
                    Column(
                        modifier = Modifier.size(80.dp, 50.dp).background(color = backgroundColor)
                    ) {
                        Text("这是KCMView")
                    }
                    Column(
                        modifier = Modifier.size(80.dp, 50.dp).background(color = backgroundColor)
                    ) {
                        Text("这是KCMView")
                    }
                    Column(
                        modifier = Modifier.size(80.dp, 50.dp).background(color = backgroundColor)
                    ) {
                        Text("这是KCMView")
                    }
                }
            }

            Row(
                horizontalArrangement = Arrangement.SpaceEvenly, // 水平方向剩余空间均匀分配
                verticalAlignment = Alignment.CenterVertically,  // 垂直方向居中
                modifier = Modifier.fillMaxWidth(1f).height(42.dp)
                    .background(color = Color.Red)
            ) {
                Box(
                    contentAlignment = Alignment.Center,
                    modifier = Modifier.size(100.dp, 36.dp).background(color = Color.Yellow)
                ) {
                    Text("点击跳转")
                }
                Box(
                    contentAlignment = Alignment.Center,
                    modifier = Modifier.size(100.dp, 36.dp).background(color = Color.Yellow)
                        .clickable {
                            uri =
                                "https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"
                        }) {
                    Text("响应更新")
                }

                var touchCount by remember { mutableStateOf(0) }
                var color by remember { mutableStateOf(Color.Blue) }

                Box(
                    contentAlignment = Alignment.Center,
                    modifier = Modifier.size(150.dp, 36.dp).background(color = Color.Yellow)
                        .touchListener(onTouchEvent = { type, position ->
                            when {
                                type == TouchType.Down -> {
                                    touchCount++
                                    color = Color.Red
                                }
                            }
                        })
                ) {
                    Text("touch次数$touchCount", color = color)
                }
            }

            var doubleTapCount by remember { mutableStateOf(0) }
            var longPressCount by remember { mutableStateOf(0) }
            var pressCount by remember { mutableStateOf(0) }
            var tabCount by remember { mutableStateOf(0) }

            Row(
                horizontalArrangement = Arrangement.SpaceEvenly, // 水平方向剩余空间均匀分配
                verticalAlignment = Alignment.CenterVertically,  // 垂直方向居中
                modifier = Modifier.fillMaxWidth(1f).height(60.dp)
                    .background(color = Color.Red)
                    .pointerInput(Unit) {
                        detectTapGestures(onDoubleTap = { offset: Offset ->
                            doubleTapCount++
                        }, onLongPress = { offset: Offset ->
                            longPressCount++
                        }, onPress = { offset: Offset ->
                            pressCount++
                        }, onTap = { offset: Offset ->
                            tabCount++
                        })
                    }) {
                Box(
                    contentAlignment = Alignment.Center,
                    modifier = Modifier.fillMaxWidth(1f).height(36.dp)
                        .background(color = Color.Yellow)
                ) {
                    Text("双击：$doubleTapCount 次 长按：$longPressCount 次 按：$pressCount 次 点击：$tabCount 次")
                }
            }
        }
    }

    @Composable
    private fun SectionTitle(title: String) {
        Text(
            text = title,
            fontSize = 18.sp,
            fontWeight = FontWeight.Bold,
            color = Color(0xFF333333)
        )
    }

    @Composable
    private fun BasicUsageDemo() {
        SectionTitle("基础用法")
        val dataList = listOf(0, 1, 2, 3, 4, 5)
        Text("基本的轮播组件展示", fontSize = 14.sp, color = Color(0xFF666666))

        Spacer(modifier = Modifier.height(8.dp))
        // 基本轮播展示 - 使用静态页面
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(200.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "BasicUsageDemo",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

    }

    @Composable
    private fun PropertyDemo() {
        val colorsAuto = listOf(
            Color(0xFF8BC34A),
            Color(0xFFCDDC39),
            Color(0xFFFFEB3B),
            Color(0xFF8BC34A),
            Color(0xFFCDDC39),
            Color(0xFFFFEB3B)
        )
        val colorsMargin = listOf(Color(0xFFFF5722), Color(0xFF795548), Color(0xFF607D8B))

        SectionTitle("属性演示")
        Text("不同配置的轮播效果", fontSize = 14.sp, color = Color(0xFF666666))

        Spacer(modifier = Modifier.height(8.dp))

        // 自动播放轮播
        Text("自动播放 + 循环", fontSize = 14.sp, color = Color(0xFF333333))
        Spacer(modifier = Modifier.height(4.dp))
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(120.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "自动播放",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

        Spacer(modifier = Modifier.height(12.dp))

        // 有间距的轮播
        Text("页面间距效果", fontSize = 14.sp, color = Color(0xFF333333))
        Spacer(modifier = Modifier.height(4.dp))
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(120.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .background(Color(0xFF03A9F4)),
                contentAlignment = Alignment.Center
            ) {
                Text(
                    "页面间距效果",
                    fontSize = 24.sp,
                    color = Color.Green
                )
            }
        }
    }

    @Composable
    private fun EventDemo() {
        var eventLog by remember { mutableStateOf("") }
        var currentPage by remember { mutableStateOf(0) }
        val eventColors = listOf(Color(0xFF9C27B0), Color(0xFF673AB7), Color(0xFF3F51B5))

        SectionTitle("事件处理")
        Text(
            "演示轮播组件的所有4种事件回调：页面选中、滚动、状态变化、滚动完成",
            fontSize = 14.sp,
            color = Color(0xFF666666)
        )

        Spacer(modifier = Modifier.height(8.dp))
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(120.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "页面间距效果",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

        Spacer(modifier = Modifier.height(8.dp))
        Text("当前页面: $currentPage", fontSize = 14.sp, color = Color(0xFF007AFF))

        Spacer(modifier = Modifier.height(4.dp))
        Text(
            text = "📍 onPageSelected: 页面选中 | 🔄 onPageScroll: 滚动中 | 🎯 onPageScrollStateChanged: 状态变化 | ✅ onPageScrollFinished: 滚动完成",
            fontSize = 10.sp,
            color = Color(0xFF666666)
        )

        // 事件日志显示
        if (eventLog.isNotEmpty()) {
            Spacer(modifier = Modifier.height(8.dp))
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .background(Color(0xFFF5F5F5), RoundedCornerShape(8.dp))
            ) {
                Text(
                    text = eventLog.take(300) + if (eventLog.length > 300) "..." else "",
                    fontSize = 12.sp,
                    color = Color(0xFF666666),
                )
            }
        }
    }

    @Composable
    private fun DataDrivenDemo() {
        // 模拟数据列表
        val dataList = remember {
            listOf(
                mapOf(
                    "title" to "轮播图1",
                    "color" to Color(0xFFE57373),
                    "desc" to "这是第一张轮播图"
                ),
                mapOf(
                    "title" to "轮播图2",
                    "color" to Color(0xFF81C784),
                    "desc" to "这是第二张轮播图"
                ),
                mapOf(
                    "title" to "轮播图3",
                    "color" to Color(0xFF64B5F6),
                    "desc" to "这是第三张轮播图"
                ),
                mapOf(
                    "title" to "轮播图4",
                    "color" to Color(0xFFFFB74D),
                    "desc" to "这是第四张轮播图"
                )
            )
        }

        SectionTitle("数据驱动示例")
        Text("使用真实数据构建轮播内容", fontSize = 14.sp, color = Color(0xFF666666))

        Spacer(modifier = Modifier.height(8.dp))
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(200.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "页面间距效果",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

        Spacer(modifier = Modifier.height(8.dp))
        Text(
            text = "💡 这个示例展示了如何使用 items() 方法根据数据列表动态生成轮播页面",
            fontSize = 12.sp,
            color = Color(0xFF666666)
        )
    }

    @Composable
    private fun DynamicDataDemo() {
        var dynamicDataList by remember {
            mutableStateOf(
                listOf(
                    mapOf(
                        "title" to "动态页面 1",
                        "desc" to "可以动态添加",
                        "color" to Color(0xFF9C27B0)
                    ),
                    mapOf(
                        "title" to "动态页面 2",
                        "desc" to "可以动态删除",
                        "color" to Color(0xFF673AB7)
                    ),
                    mapOf(
                        "title" to "动态页面 3",
                        "desc" to "数据驱动更新",
                        "color" to Color(0xFF3F51B5)
                    )
                )
            )
        }
        var operationMessage by remember { mutableStateOf("") }

        SectionTitle("动态数据管理")
        Text("演示轮播数据的动态增加、删除和更新", fontSize = 14.sp, color = Color(0xFF666666))

        Spacer(modifier = Modifier.height(8.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(160.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "页面间距效果",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

        Spacer(modifier = Modifier.height(12.dp))

        key(dynamicDataList.size) {
            Text(
                "当前轮播项数量: ${dynamicDataList.size}",
                fontSize = 14.sp,
                color = Color(0xFF333333)
            )
        }

        if (operationMessage.isNotEmpty()) {
            Spacer(modifier = Modifier.height(8.dp))
            Text(
                text = operationMessage,
                fontSize = 12.sp,
                color = Color(0xFF007AFF)
            )
        }

        Spacer(modifier = Modifier.height(12.dp))

        // 数据操作按钮
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Box(
                modifier = Modifier.clickable {
                    val newIndex = dynamicDataList.size + 1
                    val newItem = mapOf(
                        "title" to "动态页面 $newIndex",
                        "desc" to "新添加的页面",
                        "color" to Color(0xFF3F51B5)
                    )
                    dynamicDataList = dynamicDataList + newItem
                    operationMessage =
                        "添加了新页面，总数: ${dynamicDataList.size} ${DateTime.currentTimestamp()}"
                }.height(36.dp).width(80.dp)
            ) {
                Text("添加页面", fontSize = 12.sp, color = Color.White)
            }
            Box(
                modifier = Modifier.clickable {
                    if (dynamicDataList.isNotEmpty()) {
                        dynamicDataList = dynamicDataList.dropLast(1)
                        operationMessage =
                            "删除了最后一页，总数: ${dynamicDataList.size} ${DateTime.currentTimestamp()}"
                    } else {
                        operationMessage = "没有页面可删除 ${DateTime.currentTimestamp()}"
                    }
                }.height(36.dp).width(80.dp)
            ) {
                Text("删除页面", fontSize = 12.sp, color = Color.White)
            }

            Box(
                modifier = Modifier.clickable {
                    dynamicDataList = listOf(
                        mapOf(
                            "title" to "重置页面 1",
                            "desc" to "数据已重置",
                            "color" to Color(0xFFE91E63)
                        ),
                        mapOf(
                            "title" to "重置页面 2",
                            "desc" to "全新内容",
                            "color" to Color(0xFF9C27B0)
                        )
                    )
                    operationMessage =
                        "数据已重置，总数: ${dynamicDataList.size} ${DateTime.currentTimestamp()}"
                }.height(36.dp).width(80.dp)
            ) {
                Text("重置数据", fontSize = 12.sp, color = Color.White)
            }
        }
        Spacer(modifier = Modifier.height(8.dp))
        Text(
            text = "💡 这个示例展示了如何动态管理轮播数据，包括添加、删除页面和通知组件更新",
            fontSize = 12.sp,
            color = Color(0xFF666666)
        )
    }

    @Composable
    private fun MethodCallDemo() {
        var resultMessage by remember { mutableStateOf("") }
        var currentPage by remember { mutableStateOf(0) }
        val methodColors = remember {
            listOf(
                Color(0xFFFF6B6B),
                Color(0xFF4ECDC4),
                Color(0xFF45B7D1),
                Color(0xFF96CEB4),
                Color(0xFFFECEA8)
            )
        }

        SectionTitle("方法调用")
        Text("通过组件引用调用原生方法", fontSize = 14.sp, color = Color(0xFF666666))

        Spacer(modifier = Modifier.height(8.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(120.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "页面间距效果",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

        Spacer(modifier = Modifier.height(12.dp))

        Text("当前页面: $currentPage", fontSize = 14.sp, color = Color(0xFF333333))

        Spacer(modifier = Modifier.height(8.dp))

        if (resultMessage.isNotEmpty()) {
            Spacer(modifier = Modifier.height(8.dp))
            Text(
                text = resultMessage,
                fontSize = 12.sp,
                color = Color(0xFF007AFF)
            )
        }

        Spacer(modifier = Modifier.height(24.dp))

        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Box(
                modifier = Modifier.height(36.dp).width(80.dp).clickable {
                    resultMessage = "调用 scrollPageBy(-1) ${DateTime.currentTimestamp()}"
                }
            ) {
                Text("上一页", fontSize = 12.sp, color = Color.White)
            }

            Box(
                modifier = Modifier.height(36.dp).width(80.dp).clickable {
                    resultMessage = "调用 scrollPageBy(1) ${DateTime.currentTimestamp()}"
                }
            ) {
                Text("下一页", fontSize = 12.sp, color = Color.White)
            }

            Box(
                modifier = Modifier.height(36.dp).width(80.dp).clickable {
                    resultMessage = "调用 setCurrentPageIndex(2) ${DateTime.currentTimestamp()}"
                }
            ) {
                Text("跳转第3页", fontSize = 12.sp, color = Color.White)
            }

            Box(
                modifier = Modifier.height(36.dp).width(80.dp).clickable {
                    resultMessage = "调用 stopScroll() ${DateTime.currentTimestamp()}"
                }
            ) {
                Text("停止滚动", fontSize = 12.sp, color = Color.White)
            }
        }
    }

    @Composable
    private fun LazyRenderDemo() {
        var currentPage by remember { mutableStateOf(0) }
        val methodColors = listOf(
            Color(0xFF4ECDC4),
            Color(0xFF45B7D1),
            Color(0xFF96CEB4),
            Color(0xFFFECEA8),
            Color(0xFFFF6B6B)
        )
        val dataList = mutableListOf<String>()

        for (index in 0..500) {
            dataList.add("第${index + 1}页")
        }

        SectionTitle("懒加载")
        Text("beyondBoundsItemCount = 5 // 默认1", fontSize = 14.sp, color = Color(0xFF666666))

        Spacer(modifier = Modifier.height(8.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(120.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "页面间距效果",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

        Spacer(modifier = Modifier.height(12.dp))
        Text("当前页面: ${currentPage + 1}", fontSize = 14.sp, color = Color(0xFF333333))
        Spacer(modifier = Modifier.height(12.dp))

        Text(
            text = "💡 这个示例通过beyondBoundsItemCount控制懒加载，优化首屏速度",
            fontSize = 12.sp,
            color = Color(0xFF666666)
        )

        Spacer(modifier = Modifier.height(24.dp))
    }

    @Composable
    private fun NestedScrollDemo() {
        var verticalCurrentPage by remember { mutableStateOf(0) }
        var horizontalCurrentPages by remember { mutableStateOf(mutableMapOf<Int, Int>()) }

        // 生成垂直轮播的大数据列表（外层）
        val verticalDataList = remember {
            mutableListOf<Map<String, Any>>().apply {
                for (i in 0 until 100) {
                    add(
                        mapOf(
                            "id" to i,
                            "title" to "垂直页面 ${i + 1}",
                            "color" to when (i % 5) {
                                0 -> Color(0xFFE57373)
                                1 -> Color(0xFF81C784)
                                2 -> Color(0xFF64B5F6)
                                3 -> Color(0xFFFFB74D)
                                else -> Color(0xFFBA68C8)
                            }
                        )
                    )
                }
            }
        }

        SectionTitle("嵌套滚动（外层垂直 + 内层水平）")
        Text(
            "外层上下滑动，内层左右切换，测试大数据列表和懒加载",
            fontSize = 14.sp,
            color = Color(0xFF666666)
        )

        Spacer(modifier = Modifier.height(8.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(400.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }


        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "页面间距效果",
                fontSize = 24.sp,
                color = Color.Green
            )
        }


        Spacer(modifier = Modifier.height(12.dp))

        // 外层状态显示
        Text(
            text = "当前外层垂直页面: ${verticalCurrentPage + 1} / ${verticalDataList.size}",
            fontSize = 14.sp,
            color = Color(0xFF333333)
        )

        Spacer(modifier = Modifier.height(8.dp))

        Text(
            text = "💡 这个示例展示了嵌套滚动场景：外层垂直轮播包含内层水平轮播，两层都使用大数据列表测试懒加载性能",
            fontSize = 12.sp,
            color = Color(0xFF666666)
        )

        Spacer(modifier = Modifier.height(24.dp))
    }

    @Composable
    private fun GestureRenderDemo() {
        var scale by remember { mutableStateOf(1f) }
        var offset by remember { mutableStateOf(Offset.Zero) }
        var currentPage by remember { mutableStateOf(0) }

        val displayImageSize = remember { mutableStateOf(Size.Zero) } // 实际显示的图片尺寸
        val displayScale by remember(scale) {
            derivedStateOf {
                scale
            }
        }
        val displayOffset by remember {
            derivedStateOf {
                offset
            }
        }

        fun calculateBoundedOffset(
            currentOffset: Offset,
            currentScale: Float
        ): Offset {
            if (displayImageSize.value.width == 0f || displayImageSize.value.height == 0f) {
                return Offset.Zero
            }

            // 基于实际显示尺寸计算边界
            val scaledWidth = displayImageSize.value.width * currentScale
            val scaledHeight = displayImageSize.value.height * currentScale

            // 计算最大偏移量：当缩放后的尺寸大于显示尺寸时，允许偏移
            val maxOffsetX = if (currentScale <= 1f) {
                0f
            } else {
                (scaledWidth - displayImageSize.value.width) / 4
            }

            val maxOffsetY = if (currentScale <= 1f) {
                0f
            } else {
                (scaledHeight - displayImageSize.value.height) / 4
            }

            val boundedOffset = Offset(
                x = currentOffset.x.coerceIn(-maxOffsetX, maxOffsetX),
                y = currentOffset.y.coerceIn(-maxOffsetY, maxOffsetY)
            )

            return boundedOffset
        }

        val methodColors = listOf(
            Color(0xFF4ECDC4),
            Color(0xFF45B7D1),
            Color(0xFF96CEB4),
            Color(0xFFFECEA8),
            Color(0xFFFF6B6B)
        )
        val dataList = mutableListOf<String>()

        for (index in 0..500) {
            dataList.add("第${index + 1}页")
        }

        SectionTitle("手势冲突")
        Text("beyondBoundsItemCount = 5 // 默认1", fontSize = 14.sp, color = Color(0xFF666666))

        Spacer(modifier = Modifier.height(8.dp))
        var panGestureConsumedByNative = remember { mutableStateOf(true) }

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(120.dp)
                .background(Color(0xFFE8F5E8), RoundedCornerShape(8.dp))
        ) {
            Column {
                Text("1")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("2")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("3")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("4")
                Image(
                    painter = rememberAsyncImagePainter("https://wfiles.gtimg.cn/wuji_dashboard/xy/starter/baa91edc.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
            Column {
                Text("5")
                Image(
                    painter = rememberAsyncImagePainter("https://vfiles.gtimg.cn/wuji_dashboard/xy/starter/844aa82b.png"),
                    contentDescription = null,
                    Modifier.size(60.dp, 60.dp),
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF03A9F4)),
            contentAlignment = Alignment.Center
        ) {
            Text(
                "页面间距效果",
                fontSize = 24.sp,
                color = Color.Green
            )
        }

        Spacer(modifier = Modifier.height(12.dp))
        Text("当前页面: ${currentPage + 1}", fontSize = 14.sp, color = Color(0xFF333333))
        Spacer(modifier = Modifier.height(12.dp))

        Text(
            text = "💡 这个示例展示了swiper手势冲突解法，当未缩放图片时，手势会被swiper消费，当图片缩放后，手势会被图片消费",
            fontSize = 12.sp,
            color = Color(0xFF666666)
        )
        Spacer(modifier = Modifier.height(16.dp))

        // 手势拦截切换按钮
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(40.dp)
                .background(
                    if (!panGestureConsumedByNative.value) Color(0xFFE8F5E8) else Color(0xFFFFF3E0),
                    RoundedCornerShape(8.dp)
                )
                .padding(horizontal = 16.dp, vertical = 8.dp),
            contentAlignment = Alignment.CenterStart
        ) {
            Text(
                text = if (panGestureConsumedByNative.value) "图片未缩放，手势由native消费" else "图片缩放，手势由compose消费",
                fontSize = 14.sp,
                color = if (panGestureConsumedByNative.value) Color(0xFF4CAF50) else Color(
                    0xFFFF9800
                )
            )
        }

        Spacer(modifier = Modifier.height(24.dp))
    }
}